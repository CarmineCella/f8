// system.h -- system library for f8
//

#include "f8.h"

#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <chrono>
#include <future>
#include <iostream>
#include <stdexcept>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>
#include<sys/socket.h>
#include<arpa/inet.h>

namespace f8 {
    AtomPtr fn_ticks (AtomPtr params, AtomPtr env) {
        return make_node (clock ());
    }
    AtomPtr fn_dirlist (AtomPtr params, AtomPtr env) {
        std::string path = type_check (params->tail.at (0), STRING)->lexeme;
        DIR *dir;
        AtomPtr ll =  make_node();
        struct dirent *ent;
        if ((dir = opendir (path.c_str())) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                ll->tail.push_back (make_node ((std::string) "\"" + ent->d_name));
            }
            closedir (dir);
        }
        return ll;
    }
    AtomPtr fn_filestat (AtomPtr params, AtomPtr env) {
        struct stat fileStat;
        AtomPtr ll =  make_node();
        std::string filename = type_check (params->tail.at(0), STRING)->lexeme;
        int r = stat (filename.c_str (), &fileStat);
        ll->tail.push_back( make_node(r == 0));
        if (r < 0) return ll;

        ll->tail.push_back( make_node(fileStat.st_size));
        ll->tail.push_back( make_node(fileStat.st_nlink));
        std::stringstream tt;
        tt << ((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
        tt << ((fileStat.st_mode & S_IRUSR) ? "r" : "-");
        tt << ((fileStat.st_mode & S_IWUSR) ? "w" : "-");
        tt << ((fileStat.st_mode & S_IXUSR) ? "x" : "-");
        tt << ((fileStat.st_mode & S_IRGRP) ? "r" : "-");
        tt << ((fileStat.st_mode & S_IWGRP) ? "w" : "-");
        tt << ((fileStat.st_mode & S_IXGRP) ? "x" : "-");
        tt << ((fileStat.st_mode & S_IROTH) ? "r" : "-");
        tt << ((fileStat.st_mode & S_IWOTH) ? "w" : "-");
        tt << ((fileStat.st_mode & S_IXOTH) ? "x" : "-");
        ll->tail.push_back(make_node((std::string) "\"" + tt.str ().c_str ()));
        return ll;
    }
    AtomPtr fn_getvar (AtomPtr params, AtomPtr env) {
        char* c = getenv (type_check (params->tail.at (0), STRING)->lexeme.c_str ());
        if (c) return make_node ((std::string) "\"" + (std::string) c);
        else return make_node("");
    }
    #define MESSAGE_SIZE 4096
    AtomPtr fn_udprecv (AtomPtr n, AtomPtr env) {
        struct sockaddr_in server, client;
        char client_message[MESSAGE_SIZE];
        memset (client_message, 0, sizeof (char) * MESSAGE_SIZE);

        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock == -1) {
            return  make_node (0);
        }

        server.sin_addr.s_addr = inet_addr(type_check (n->tail.at(0), STRING)->lexeme.c_str ());
        server.sin_family = AF_INET;
        server.sin_port = htons((long)type_check (n->tail.at(1), NUMERIC)->val[0]);
    
    if(::bind(sock,(struct sockaddr *)&server , sizeof(server)) < 0) {
            return make_node(0);
        }
        int c = sizeof(struct sockaddr_in);
        if (recvfrom(sock, client_message, MESSAGE_SIZE, 0, 
            (struct sockaddr *) &client, (socklen_t*) &c) < 0) {
            return  make_node(0);   
        }

        ::close (sock);
        return make_node ((std::string) "\"" + client_message);
    }
    AtomPtr fn_udpsend (AtomPtr n, AtomPtr env) {
        struct sockaddr_in server;
        char server_reply[MESSAGE_SIZE];
        memset (server_reply, 0, sizeof (char) * MESSAGE_SIZE);

        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock == -1) {
            return  make_node(0);
        }
        
        server.sin_addr.s_addr = inet_addr(type_check (n->tail.at (0), STRING)->lexeme.c_str ());
        server.sin_family = AF_INET;
        server.sin_port = htons((long)type_check (n->tail.at(1), NUMERIC)->val[0]);

        std::stringstream nf;
        puts (n->tail.at(2), nf);

        if (sendto(sock, nf.str ().c_str (), n->tail.at(2)->lexeme.size (), 0, 
            (struct sockaddr *)&server , sizeof(server)) < 0) {
            return  make_node(0);
        }
        ::close (sock);
        return  make_node (1);
    }
    // I/O  -----------------------------------------------------------------------
    AtomPtr fn_openstream (AtomPtr node, AtomPtr env) {
        std::string name = type_check (node->tail.at(0), STRING)->lexeme;
        std::string direction = type_check (node->tail.at(1), SYMBOL)->lexeme;

        bool input = false;
        if (direction == "input") input = true;
        else if (direction == "output") input = false;
        else Context::error ("[openstream] unsopported direction", node);
        AtomPtr s =  make_node();
        AtomPtr ll =   make_node();
        ll->tail.push_back (make_node ((std::string) "\"" + name));

        if (input) {
            std::istream* f = nullptr;
            f = new std::ifstream (name);
            if (!f->good ()) return make_node (0);
            s = ( make_obj ("instream", f, ll));
        }
        else {
            std::ostream* f = nullptr;
            f = new std::ofstream (name);
            if (!f->good ()) return make_node (0);
            s = ( make_obj ("outstream", f, ll));        
        }
        return s;
    }
    AtomPtr fn_closestream (AtomPtr node, AtomPtr env) {
        AtomPtr p = type_check (node->tail.at(0), OBJECT);
        if (p->obj == 0) return  make_node();
        if (p->lexeme == "instream") {
            std::ifstream* istr = static_cast<std::ifstream*> (p->obj);
            istr->close ();
            delete istr;
            p->obj = 0;
            return  make_node (1);
        } else if (p->lexeme == "outstream") {
            std::ofstream* ostr = static_cast<std::ofstream*> (p->obj);
            ostr->close ();
            delete ostr;
            p->obj = 0;
            return  make_node(1);
        }
        return  make_node(0);
    }
    AtomPtr fn_isgood (AtomPtr node, AtomPtr env) {
        AtomPtr p = type_check (node->tail.at(0), OBJECT);
        if (p->obj == 0) return  make_node(0);
        if (p->lexeme == "instream") {
            return  make_node(static_cast<std::istream*> (p->obj)->good () ? 1 : 0);
        } else if (p->lexeme == "outstream") {
            return  make_node(static_cast<std::ostream*> (p->obj)->good () ? 1 : 0);
        }
        return make_node(0);
    }
    AtomPtr fn_rwndstream (AtomPtr node, AtomPtr env) {
        AtomPtr p = type_check (node->tail.at(0), OBJECT);
        if (p->obj == 0) return  make_node(0);
        if (p->lexeme == "instream") {
            std::istream* istr = static_cast<std::istream*>(p->obj);
            istr->clear();
            istr->seekg (0);
            return  make_node (1);
        } else if (p->lexeme == "outstream") {
            std::ostream* ostr = static_cast<std::ostream*>(p->obj);
            ostr->clear();
            ostr->seekp (0);
            return  make_node(1);
        }
        return  make_node(0);
    }
    AtomPtr fn_writeline (AtomPtr n, AtomPtr env) {
        AtomPtr p = type_check (n->tail.at(0), OBJECT);
        if (p->obj == 0 || p->lexeme != "outstream") return  make_node(0);
        std::ostream* out = static_cast<std::ostream*> (p->obj);
        for (unsigned  i = 1; i < n->tail.size (); ++i)  {
            *out << type_check (n->tail.at (i), STRING)->lexeme;
            out->flush();
        }
        return make_node (1);    
    }
    AtomPtr fn_readline (AtomPtr n, AtomPtr env) {
        AtomPtr p = type_check (n->tail.at(0), OBJECT);
        if (p->obj == 0 || p->lexeme != "instream") return  make_node(0);
        std::istream* in = static_cast<std::istream*> (p->obj);
        std::string name = p->tail.at(0)->lexeme; // exists by default
        if (!in->good () || in->eof ()) return  make_node(0);
        std::string buff;
        std::getline (*in, buff);
        return make_node ((std::string) "\"" + buff);
    }
    AtomPtr add_system (AtomPtr env) {
        add_operator ("ticks", &fn_ticks, 0, env);
        add_operator ("dirlist", &fn_dirlist, 1, env);
        add_operator ("filestat", &fn_filestat, 1, env);
        add_operator ("getvar", &fn_getvar, 1, env);
        add_operator ("udpsend", &fn_udpsend, 3, env);
        add_operator ("udprecv", &fn_udprecv, 2, env);
        add_operator ("openstream", &fn_openstream, 2, env);
        add_operator ("closestream", &fn_closestream, 1, env);
        add_operator ("isgood", &fn_isgood, 1, env);
        add_operator ("rewindstream", &fn_rwndstream, 1, env);
        add_operator ("writeline", &fn_writeline, 2, env);
        add_operator ("readline", &fn_readline, 1, env);
        return env;
    }
}

// eof
