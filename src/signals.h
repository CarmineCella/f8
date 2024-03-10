// signals.h
// 

#ifndef SIGNALS_H
#define SIGNALS_H 

#include "signals/BPF.h"
#include "signals/WavFile.h"
#include "signals/FFT.h"

#include "f8.h"

#include <vector>
#include <valarray>

namespace f8 {
	// support  -----------------------------------------------------------------------
	void gen10 (const std::valarray<Real>& coeff, std::valarray<Real>& values) {
		for (unsigned i = 0; i < values.size () - 1; ++i) {
			values[i] = 0;
			for (unsigned j = 0; j < coeff.size (); ++j) {
				values[i] += coeff[j] * sin (2. * M_PI * (j + 1) * (Real) i / values.size ());
			}
			values[i] /= coeff.size ();
		}
		values[values.size () - 1] = values[0]; // guard point
	}	
	int next_pow2 (int n) {
		if (n == 0 || ceil(log2(n)) == floor(log2(n))) return n;
		int count = 0;
		while (n != 0) {
			n = n>>1;
			count = count + 1;
		}
		return (1 << count) ;
	}
	template <typename T>
	void complexMultiplyReplace (
		const T* src1, const T* src2, T* dest, int num) {
		while (num--) {
			T r1 = *src1++;
			T r2 = *src2++;
			T i1 = *src1++;
			T i2 = *src2++;

			*dest++ = r1*r2 - i1*i2;
			*dest++ = r1*i2 + r2*i1;
		}
	}
	template <typename T>
	void interleave (T* stereo, const T* l, const T* r, int n) {
		for (int i = 0; i < n; ++i) {
			stereo[2 * i] = l[i];
			stereo[2 * i + 1] = r[i];
		}
	}
	template <typename T>
	void deinterleave (const T* stereo, T* l, T* r, int n) {
		for (int i = 0; i < n / 2; ++i) {
			l[i] = stereo[2 * i];
			r[i] = stereo[2 * i + 1];
		}
	}	
	std::valarray<Real> to_array (AtomPtr list) {
		int sz = list->tail.size ();
		std::valarray<Real> out (sz);
		for (unsigned i = 0; i < sz; ++i) out[i] = type_check (list->tail.at (i), NUMERIC)->val[0];
		return out;
	}
	// numeric --------------------------------------------------------------------------------------
	AtomPtr fn_bpf (AtomPtr node, AtomPtr env) {
		Real init = type_check (node->tail.at (0), NUMERIC)->val[0];
		int len  = (int) type_check (node->tail.at (1), NUMERIC)->val[0];
		Real end = type_check (node->tail.at (2), NUMERIC)->val[0];
		node->tail.pop_front (); node->tail.pop_front (); node->tail.pop_front ();
		if (node->tail.size () % 2 != 0) Context::error ("[bpf] invalid number of arguments", node);
		BPF<Real> bpf (len);
		bpf.add_segment (init, len, end);
		Real curr = end;
		for (unsigned i = 0; i < node->tail.size () / 2; ++i) {
			int len  = (int) type_check (node->tail.at (i * 2), NUMERIC)->val[0];
			Real end = type_check (node->tail.at (i * 2 + 1), NUMERIC)->val[0];
			bpf.add_segment (curr, len, end);
			curr = end;
		}
		std::valarray<Real> out;
		bpf.process (out);
		return make_node (out);
	}
	AtomPtr fn_mix (AtomPtr node, AtomPtr env) {
		std::vector<Real> out;
		if (node->tail.size () % 2 != 0) Context::error ("[mix] invalid number of arguments", node);
		for (unsigned i = 0; i < node->tail.size () / 2; ++i) {
			int p = (int) type_check (node->tail.at (i * 2), NUMERIC)->val[0];
			AtomPtr l = type_check (node->tail.at (i * 2 + 1), NUMERIC);
			int len = (int) (p + l->val.size ());
			if (len > out.size ()) out.resize (len, 0);
			// out[std::slice(p, len, 1)] += l->array;
			for (unsigned t = 0; t < l->val.size (); ++t) {
				out[t + p] += l->val[t];
			}
		}
		std::valarray<Real> v (out.data(), out.size());
		return make_node (v);
	}
	AtomPtr fn_gen (AtomPtr node, AtomPtr env) {
		int len = (int) type_check (node->tail.at (0), NUMERIC)->val[0];
		std::valarray<Real> coeffs (node->tail.size () - 1);
		for (unsigned i = 1; i < node->tail.size (); ++i) {
			coeffs[i - 1] = ((type_check (node->tail.at (i), NUMERIC)->val[0]));
		}
		Real init = 0; 
		std::valarray<Real> table (init, len + 1); 
		gen10 (coeffs, table);
		return make_node (table);
	}
	AtomPtr fn_osc (AtomPtr node, AtomPtr env) {
		Real sr = type_check (node->tail.at (0), NUMERIC)->val[0];
		std::valarray<Real>& freqs = type_check (node->tail.at (1), NUMERIC)->val;
		std::valarray<Real>& table = type_check (node->tail.at (2), NUMERIC)->val;
		Real init = 0;
		std::valarray<Real> out (init, freqs.size ());
		int N = table.size () - 1;
		Real fn = (Real) sr / N; // Hz
		Real phi = 0; //rand () % (N - 1);
		for (unsigned i = 0; i < freqs.size (); ++i) {
			int intphi = (int) phi;
			Real fracphi = phi - intphi;
			Real c = (1 - fracphi) * table[intphi] + fracphi * table[intphi + 1];
			out[i] = c;
			phi = phi + freqs[i] / fn;
			if (phi >= N) phi = phi - N;
		}
		return make_node (out);
	}
	AtomPtr fn_reson (AtomPtr node, AtomPtr env) {
		std::valarray<Real>& array = type_check (node->tail.at (0), NUMERIC)->val;
		Real sr = type_check (node->tail.at (1), NUMERIC)->val[0];
		Real freq = type_check (node->tail.at (2), NUMERIC)->val[0];
		Real tau = type_check (node->tail.at (3), NUMERIC)->val[0];
		
		Real om = 2 * M_PI * (freq / sr);
		Real B = 1. / tau;
		Real t = 1. / sr;
		Real radius = exp (-2. * M_PI * B * t);
		Real a1 = -2 * radius * cos (om);
		Real a2 = radius * radius;
		Real gain = radius * sin (om);

		int samps = (int) (sr * tau);
		std::valarray<Real> out (samps);
		int insize = array.size ();

		Real x1 = 0;
		Real y1 = 0;
		Real y2 = 0;
		for (unsigned i = 0; i < samps; ++i) {
			Real v = gain * x1 - (a1 * y1) - (a2 * y2);
			x1 = i < insize ? array[i] : 0;
			y2 = y1;
			y1 = v;
			out[i] = v;
		}
		return make_node (out);
	}
	template <int sign>
	AtomPtr fn_fft (AtomPtr n, AtomPtr env) {
		std::valarray<Real>& sig = type_check (n->tail.at (0), NUMERIC)->val;
		int d = sig.size ();
		int N = next_pow2 (d);
		int norm = (sign < 0 ? 1 : N / 2);
		std::valarray<Real> inout (N);
		for (unsigned i = 0; i < d; ++i) inout[i] = sig[i];
		fft<Real> (&inout[0], N / 2, sign);
		
		for (unsigned i = 0; i < N; ++i) inout[i] /= norm;	
		return make_node (inout);
	}
	AtomPtr fn_car2pol (AtomPtr n, AtomPtr env) {
		std::valarray<Real>& inout = type_check (n->tail.at (0), NUMERIC)->val;
		rect2pol (&inout[0], inout.size () / 2);
		return make_node (inout);
	}
	AtomPtr fn_pol2car (AtomPtr n, AtomPtr env) {
		std::valarray<Real>& inout = type_check (n->tail.at (0), NUMERIC)->val;
		pol2rect (&inout[0], inout.size () / 2);
		return make_node (inout);
	}
	AtomPtr fn_conv (AtomPtr n, AtomPtr env) {
		std::valarray<Real>& ir = type_check (n->tail.at (0), NUMERIC)->val;
		std::valarray<Real>& sig = type_check (n->tail.at (1), NUMERIC)->val;
		Real scale = type_check(n->tail.at (2), NUMERIC)->val[0];
		Real mix = 0;
		if (n->tail.size () == 4) mix = type_check(n->tail.at (3), NUMERIC)->val[0];
		long irsamps = ir.size ();
		long sigsamps = sig.size ();
		if (irsamps <= 0 || sigsamps <= 0) Context::error ("[conv] invalid lengths", n);
		int max = irsamps > sigsamps ? irsamps : sigsamps;
		int N = next_pow2(max) << 1;
		std::valarray<Real> fbuffir(2 * N);
		std::valarray<Real> fbuffsig(2 * N);
		std::valarray<Real> fbuffconv(2 * N);
		for (unsigned i = 0; i < N; ++i) {
			if (i < irsamps) fbuffir[2 * i] = ir[i];
			else fbuffir[2 * i] = 0;
			if (i < sigsamps) fbuffsig[2 * i] = sig[i];
			else fbuffsig[2 * i] = 0;
			fbuffconv[2 * i] = 0;
			fbuffir[2 * i + 1] = 0;
			fbuffsig[2 * i + 1] = 0;
			fbuffconv[2 * i + 1] = 0;        
		}
		AbstractFFT<Real>* fft = createFFT<Real>(N);
		fft->forward(&fbuffir[0]);
		fft->forward(&fbuffsig[0]);
		complexMultiplyReplace(&fbuffir[0], &fbuffsig[0], &fbuffconv[0], N);
		fft->inverse(&fbuffconv[0]);
		std::valarray<Real> out  (irsamps + sigsamps - 1);
		for (unsigned i = 0; i < (irsamps + sigsamps) -1; ++i) {
			Real s = scale * fbuffconv[2 * i] / N;
			if (i < sigsamps) s+= sig[i] * mix;
			out[i] = s;
		}
		return make_node (out);
	}
	AtomPtr fn_noise (AtomPtr n, AtomPtr env) {
		int len = (int) type_check (n->tail.at (0), NUMERIC)->val[0];
		int rows = 1;
		if (n->tail.size () == 2) rows = (int) type_check (n->tail.at (1), NUMERIC)->val[0];
		AtomPtr l = make_node ();
		for (unsigned j = 0; j < rows; ++j) {
			std::valarray<Real> out (len);
			for (unsigned i = 0; i < len; ++i) out[i] = ((Real) rand () / RAND_MAX) * 2. - 1;
			l->tail.push_back (make_node (out));
		}
		return l->tail.size () == 1 ? l->tail.at (0) : l;
	}
	// I/O  -----------------------------------------------------------------------
	AtomPtr fn_openwav (AtomPtr node, AtomPtr env) {
		std::string name = type_check (node->tail.at(0), STRING)->lexeme;
		std::string direction = type_check (node->tail.at(1), SYMBOL)->lexeme;
		int sr = 44100; 
		int ch = 1; 

		bool input = false;

		if (direction == "input") input = true;
		else if (direction == "output") input = false;
		else Context::error ("unsopported direction for stream", node);
		
		if (input == false) {
			if (node->tail.size () == 4) {
				sr = type_check (node->tail.at(2), NUMERIC)->val[0];
				ch = type_check (node->tail.at(3), NUMERIC)->val[0];
				if (ch > 2) Context::error ("[openwav] only stereo files are supported", node);
 			} else Context::error ("[openwav] missing sr and ch for output wav", node);
		}
		AtomPtr s = make_node();
		AtomPtr ll =  make_node();
		ll->tail.push_back (make_node((std::string) "\"" + name));

		if (input) {
			WavInFile* f = new WavInFile (name.c_str ());
			s = (make_obj ("inwav", f, ll));
		}
		else {
			WavOutFile* f = new WavOutFile (name.c_str (), sr, 16, ch);
			s = (make_obj ("outwav", f, ll));        
		}
		return s;
	}
	AtomPtr fn_readwav (AtomPtr node, AtomPtr env) {
		AtomPtr p = type_check (node->tail.at(0), OBJECT);
		if (p->obj == 0 || p->lexeme != "inwav") Context::error ("[readwav] cannot read an input file", node);
		WavInFile* in = static_cast<WavInFile*> (p->obj);

		AtomPtr final = make_node();
		long sz = in->getNumSamples ();
		long ch = in->getNumChannels ();
		if (ch > 2) Context::error ("[openwav] only stereo files are supported", node);

		if (node->tail.size () == 2) sz = type_check (node->tail.at(1), NUMERIC)->val[0];

		std::valarray<Real> data (sz * ch);
		in->read (&data[0], sz * ch);
		if (ch == 1) {
			final->tail.push_back (make_node (data));
		} else if (ch == 2) {
			std::valarray<Real> left (sz);
			std::valarray<Real> right (sz);
			deinterleave (&data[0], &left[0], &right[0], ch * sz);
			final->tail.push_back (make_node (left));
			final->tail.push_back (make_node (right));
		}
		return final;
	}
	AtomPtr fn_infowav (AtomPtr node, AtomPtr env) {
		AtomPtr p = type_check (node->tail.at(0), OBJECT);
		if (p->obj == 0 || p->lexeme != "inwav") Context::error ("[infowav] cannot get info for an output file", node);
		WavInFile* in = static_cast<WavInFile*> (p->obj);

		AtomPtr final = make_node ();
		final->tail.push_back (make_node (in->getSampleRate ()));
		final->tail.push_back (make_node (in->getNumChannels ()));
		final->tail.push_back (make_node (in->getNumSamples ()));
		
		return final;
	}
	AtomPtr fn_writewav (AtomPtr node, AtomPtr env) {
		AtomPtr p = type_check (node->tail.at(0), OBJECT);
		if (p->obj == 0 || p->lexeme != "outwav") Context::error ("[writewav] cannot write an output file", node);
		WavOutFile* out = static_cast<WavOutFile*> (p->obj);

		AtomPtr final = type_check (node->tail.at (1), LIST);
		if (final->tail.size () > 2 || !final->tail.size ()) Context::error ("[writewav] only mono/stereo files are supported", node);		
		int sz = final->tail.at (0)->val.size ();
		int ch = final->tail.size ();

		if (ch == 1) {
			out->write (&final->tail.at (0)->val[0], sz);
		} else if (ch == 2) {
			std::valarray<Real> data (sz * ch);
			interleave (&data[0], &final->tail.at (0)->val[0], &final->tail.at (1)->val[0], sz);
			out->write (&data[0], ch * sz);
 		}
		return make_node (sz * ch);
	}
	AtomPtr fn_closewav (AtomPtr node, AtomPtr env) {
		AtomPtr p = type_check (node->tail.at(0), OBJECT);
		if (p->obj == 0) return make_node();
		if (p->lexeme == "inwav") {
			WavInFile* istr = static_cast<WavInFile*> (p->obj);
			delete istr;
			p->obj = 0;
			return make_node(1);
		} else if (p->lexeme == "outwav") {
			WavOutFile* ostr = static_cast<WavOutFile*> (p->obj);
			delete ostr;
			p->obj = 0;
			return make_node(1);
		}
		return make_node(0);
	}	
	AtomPtr add_signals (AtomPtr env) {
		add_operator ("bpf", fn_bpf, 3, env);
		add_operator ("mix", fn_mix, 2, env);	
		add_operator ("gen", fn_gen, 2, env);
		add_operator ("osc", fn_osc, 3, env);
		add_operator ("reson", fn_reson, 3, env);
		add_operator ("fft", fn_fft<1>, 1, env);
		add_operator ("ifft", fn_fft<-1>, 1, env);
		add_operator ("car2pol", fn_car2pol, 1, env);
		add_operator ("pol2car", fn_pol2car, 1, env);
		add_operator ("conv", fn_conv, 3, env);
		add_operator ("noise", fn_noise, 1, env);
		add_operator ("openwav", fn_openwav, 2, env);
		add_operator ("writewav", fn_writewav, 2, env);
		add_operator ("readwav", fn_readwav, 1, env);
		add_operator ("infowav", fn_infowav, 1, env);
		add_operator ("closewav", fn_closewav, 1, env);
		return env;
	}
}
#endif	// NUMERIC_H 

// EOF
