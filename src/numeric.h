// numeric.h
// 

#ifndef NUMERIC_H
#define NUMERIC_H 

#include "BPF.h"
#include "WavFile.h"
#include "FFT.h"

#include "f8.h"

#include <vector>
#include <valarray>

// SUPPORT  -----------------------------------------------------------------------
void gen10 (const std::valarray<Real>& coeff, std::valarray<Real>& values) {
	for (unsigned i = 0; i < values.size () - 1; ++i) {
		values[i] = 0;
		for (unsigned j = 0; j < coeff.size (); ++j) {
			values[i] += coeff[j] * sin (2. * M_PI * (j + 1) * (float) i / values.size ());
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
	for (unsigned i = 0; i < sz; ++i) out[i] = type_check (list->tail.at (i), NUMBER)->val;
	return out;
}
// NUMERIC --------------------------------------------------------------------------------------
AtomPtr fn_bpf (AtomPtr node, AtomPtr env) {
	Real init = type_check (node->tail.at (0), NUMBER)->val;
	int len  = (int) type_check (node->tail.at (1), NUMBER)->val;
	Real end = type_check (node->tail.at (2), NUMBER)->val;
	node->tail.pop_front (); node->tail.pop_front (); node->tail.pop_front ();
	if (node->tail.size () % 2 != 0) error ("invalid number of arguments for bpf", node);
	BPF<Real> bpf (len);
	bpf.add_segment (init, len, end);
	Real curr = end;
	for (unsigned i = 0; i < node->tail.size () / 2; ++i) {
		int len  = (int) type_check (node->tail.at (i * 2), NUMBER)->val;
		Real end = type_check (node->tail.at (i * 2 + 1), NUMBER)->val;
		bpf.add_segment (curr, len, end);
		curr = end;
	}
	std::valarray<Real> out;
	bpf.process (out);
	return make_node (out);
}
AtomPtr fn_mix (AtomPtr node, AtomPtr env) {
	std::vector<Real> out;
	if (node->tail.size () % 2 != 0) error ("invalid number of arguments for mix", node);
	for (unsigned i = 0; i < node->tail.size () / 2; ++i) {
		int p = (int) type_check (node->tail.at (i * 2), NUMBER)->val;
		AtomPtr l = type_check (node->tail.at (i * 2 + 1), LIST);
		int len = (int) (p + l->tail.size ());
		if (len > out.size ()) out.resize (len, 0);
		// out[std::slice(p, len, 1)] += l->array;
		for (unsigned t = 0; t < l->tail.size (); ++t) {
			out[t + p] += type_check (l->tail[t], NUMBER)->val;
		}
	}
	std::valarray<Real> v (out.data(), out.size());
	return make_node (v);
}
AtomPtr fn_gen (AtomPtr node, AtomPtr env) {
	int len = (int) type_check (node->tail.at (0), NUMBER)->val;
	std::valarray<Real> coeffs = to_array (type_check (node->tail.at (1), LIST));
	std::valarray<Real> table (len + 1); 
	gen10 (coeffs, table);
	return make_node (table);
}
AtomPtr fn_osc (AtomPtr node, AtomPtr env) {
	Real sr = type_check (node->tail.at (0), NUMBER)->val;
	std::valarray<Real> freqs = to_array (type_check (node->tail.at (1), LIST));
	std::valarray<Real> table = to_array (type_check (node->tail.at (2), LIST));
	std::valarray<Real> out (freqs.size ());
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
	std::valarray<Real> array = to_array (type_check (node->tail.at (0), LIST));
	Real sr = type_check (node->tail.at (1), NUMBER)->val;
	Real freq = type_check (node->tail.at (2), NUMBER)->val;
	Real tau = type_check (node->tail.at (3), NUMBER)->val;
	
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
	AtomPtr sig = type_check (n->tail.at (0), LIST);
	int d = sig->tail.size ();
	int N = next_pow2 (d);
	int norm = (sign < 0 ? 1 : N / 2);
    std::valarray<Real> inout (N);
	for (unsigned i = 0; i < d; ++i) inout[i] = sig->tail[i]->val;
    fft<Real> (&inout[0], N / 2, sign);
  	
	for (unsigned i = 0; i < N; ++i) inout[i] /= norm;	
	return make_node (inout);
}
AtomPtr fn_car2pol (AtomPtr n, AtomPtr env) {
	std::valarray<Real> inout = to_array (type_check (n->tail.at (0), LIST));
	rect2pol (&inout[0], inout.size () / 2);
	return make_node (inout);
}
AtomPtr fn_pol2car (AtomPtr n, AtomPtr env) {
 	std::valarray<Real> inout = to_array (type_check (n->tail.at (0), LIST));
	pol2rect (&inout[0], inout.size () / 2);
	return make_node (inout);
}
AtomPtr fn_conv (AtomPtr n, AtomPtr env) {
    std::valarray<Real> ir = to_array (type_check (n->tail.at (0), LIST));
    std::valarray<Real> sig = to_array (type_check (n->tail.at (1), LIST));
    Real scale = type_check(n->tail.at (2), NUMBER)->val;
	Real mix = 0;
	if (n->tail.size () == 4) mix = type_check(n->tail.at (3), NUMBER)->val;
    long irsamps = ir.size ();
    long sigsamps = sig.size ();
    if (irsamps <= 0 || sigsamps <= 0) error ("invalid lengths for conv", n);
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
 	int len = (int) type_check (n->tail.at (0), NUMBER)->val;
	std::valarray<Real> out (len);
	for (unsigned i = 0; i < len; ++i) out[i] = ((Real) rand () / RAND_MAX) * 2. - 1;
	return make_node (out);
}
// I/O  -----------------------------------------------------------------------
AtomPtr fn_sndwrite (AtomPtr node, AtomPtr env) {
	Real sr = type_check (node->tail.at (0), NUMBER)->val;
	std::valarray<Real> vals;
	if (node->tail.size () == 3) {
		WavOutFile outf (type_check (node->tail.at (1), STRING)->lexeme.c_str(), sr, 16, 1);
		vals = to_array (type_check (node->tail.at (2), LIST));
		outf.write (&vals[0], vals.size ());
	} else if (node->tail.size () == 4) {
		WavOutFile outf (type_check (node->tail.at (1), STRING)->lexeme.c_str(), sr, 16, 2);
		std::valarray<Real> left = to_array (type_check (node->tail.at (2), LIST));
		std::valarray<Real> right = to_array (type_check (node->tail.at (3), LIST));
		vals.resize (2 * left.size ());
		interleave (&vals[0], &left[0], &right[0], left.size ());
		outf.write (&vals[0], vals.size ());
	} else error ("invalid number of channels in", node->tail.at(0));
	
	return make_node (vals.size ());
}
AtomPtr fn_sndread (AtomPtr node, AtomPtr env) {
	WavInFile infile (type_check (node->tail.at (0), STRING)->lexeme.c_str());
	AtomPtr l = make_node ();
	int s = infile.getNumSamples ();
	std::valarray<Real> input (s);
	infile.read (&input[0], s);
	std::valarray<Real> info (3);
	info[0] = infile.getSampleRate ();
	info[1] = infile.getNumChannels ();
	info[2] = s;
	l->tail.push_back (make_node (info));
	if (infile.getNumChannels () == 1) {
		l->tail.push_back (make_node (input));
	} else if (infile.getNumChannels () == 2) {
		std::valarray<Real> left (s/2);
		std::valarray<Real> right (s/2);
		deinterleave (&input[0], &left[0], &right[0], s);
		l->tail.push_back (make_node (left));
		l->tail.push_back (make_node (right));
	} else error ("invalid number of channels in", node->tail.at (0));
	return l;
}
void add_numeric (AtomPtr env) {
	add_builtin ("bpf", fn_bpf, 3, env);
	add_builtin ("mix", fn_mix, 2, env);	
	add_builtin ("gen", fn_gen, 2, env);
	add_builtin ("osc", fn_osc, 3, env);
	add_builtin ("reson", fn_reson, 3, env);
	add_builtin ("fft", fn_fft<1>, 1, env);
	add_builtin ("ifft", fn_fft<-1>, 1, env);
	add_builtin ("car2pol", fn_car2pol, 1, env);
	add_builtin ("pol2car", fn_pol2car, 1, env);
	add_builtin ("conv", fn_conv, 3, env);
	add_builtin ("noise", fn_noise, 1, env);
	// // I/O
	add_builtin ("sndwrite", fn_sndwrite, 3, env);
	add_builtin ("sndread", fn_sndread, 1, env);
}
#endif	// NUMERIC_H 

// EOF
