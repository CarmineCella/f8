// FIRDesigner.h
// 

#ifndef FIRDESIGNER_H
#define FIRDESIGNER_H

#include <cmath>

namespace f8 {
	template <typename T>
	class FIRDesigner {
		void invert_spectrum (T* h, int N) {
			for (int i = 0; i < N; ++i) {
				 h[i] = -h[i];
			}
			h[(N - 1) / 2] += 1.;
		}		
	public:
		void lowpass (T* h, int N, T sr, T cutoff) {
			Real lambda = M_PI * cutoff / (sr / 2.);
			for(unsigned n = 0; n < N; ++n){
				T mm = n - (N - 1.0) / 2.0;
				if (mm == 0.0 ) h[n] = lambda / M_PI;
				else h[n] = sin (mm * lambda) / (mm * M_PI);
			}			
		}
		void highpass (T* h, int N, T sr, T cutoff) {
			lowpass (h, N, sr, cutoff);
			invert_spectrum (h, N);
		}
		void bandpass (T* h, int N, T sr, T cutoff_low, T cutoff_high) {
			Real lambda = M_PI * cutoff_low / (sr/2);
			Real phi = M_PI * cutoff_high / (sr/2);			
			for(unsigned n = 0; n < N; ++n){
				Real mm = n - (N - 1.0) / 2.0;
				if (mm == 0.0) h[n] = (phi - lambda) / M_PI;
				else h[n] = (sin (mm * phi) - sin (mm * lambda)) / (mm * M_PI);
			}
		}
		void bandstop (T* h, int N, T sr, T cutoff_low, T cutoff_high) {
			bandpass (h, N, sr, cutoff_low, cutoff_high);
			invert_spectrum (h, N);
		}
    };
}

#endif	// FIRDESIGNER_H 

// EOF
