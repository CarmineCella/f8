// algorithms.h
//

#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <cmath>
#include <limits>
#include <float.h>
#include <stdexcept>
#include <cstring>
#include <cassert>

namespace f8 {
	#define PCASIGN(a, b) ( (b) < 0 ? -fabs(a) : fabs(a) )
	#define EPS .0000000000001

	// ------------------------------------------------------------------//

	template <typename T>
	void swapElem(T& a, T& b) {
		T t = (a);
		(a) = (b);
		(b) = t;
	}

	template <typename T>
	T kth_smallest(T a[], int n, int k) {
		int i, j, l, m;
		T x;
		l = 0;
		m = n - 1;
		while (l < m) {
			x = a[k];
			i = l;
			j = m;
			do {
				while (a[i] < x) i++;
				while (x < a[j]) j--;
				if (i <= j) {
					swapElem(a[i], a[j]);
					i++;
					j--;
				}
			} while (i <= j);
			if (j < k) l = i;
			if (k < i) m = j;
		}
		return a[k];
	}

	template <typename T>
	T median (T a[], int n) {
		return kth_smallest(a, n, (((n) & 1) ? ((n) / 2) : (((n) / 2) - 1)));
	}

	template <typename T>
	inline T moment(
			const T* weights,
			const T* values,
			int N,
			int order,
			T centroid) {
		T sumWeigth = 0;
		T sumWeigthDistance = 0;
		T distance = 0;
		T weigth = 0;
		T weigthDistance = 0;

		for (int index = 0; index < N; ++index) {
			distance = values[index];
			distance -= centroid;
			distance = std::pow(distance, order);
			weigth = weights[index];
			weigthDistance = weigth * distance;
			sumWeigth += weigth;
			sumWeigthDistance += weigthDistance;
		}

		return (sumWeigth != 0 ? sumWeigthDistance / sumWeigth : 0);
	}

	template <typename T>
	static inline T linreg(
			const T* weights,
			const T* values,
			int N,
			T& step) {
		int index;
		int nbValue = N;

		T Xk = 0;
		T Yk = 0;
		T Xk2 = 0;
		T XkYk = 0;

		T sumXk = 0;
		T sumYk = 0;
		T sumXk2 = 0;
		T sumXkYk = 0;

		T slope = 0;

		for (index = 0; index < nbValue; index++) {
			Xk = values[index];
			Yk = weights[index];
			Xk2 = Xk * Xk;
			XkYk = Xk * Yk;

			sumXk += Xk;
			sumYk += Yk;
			sumXkYk += XkYk;
			sumXk2 += Xk2;
		}

		T numSlope = ((T) nbValue) * sumXkYk - (sumXk * sumYk);
		T numStep = sumXk2 * sumYk - sumXk * sumXkYk;
		T denSlope = ((T) nbValue) * sumXk2 - std::pow((T)sumXk, (T)2.0);

		if (0 != denSlope) {
			slope = numSlope / denSlope;
			step = numStep / denSlope;
		} else {
			slope = 0;
			step = 0;
		}
		return slope;
	}

	// -------------------------------------------------------------- //

	template <typename T>
	T edistance (const T* a, const T* b, int size) {
		// NB: it assumes vector have the same size
		T sum = 0;

		for (int i = 0; i < size; ++i) {
			T s = a[i] - b[i];
			sum += s * s;
		}
		if (std::isnan(sum) || std::isinf(sum)) return 0;
		else return std::sqrt(sum);
	}

	template <typename T>
	T mahalanobis (const T* a, const T* b, int size) {
		// NB: it assumes vector have the same size
		T* w = new T[size];
		for (int i = 0; i < size; ++i) {
			w[i] = (T) 1. / size;
		}

		T m = mean(a, size);
		T stdd = stddev(w, a, m, size);

		T v = 0, d = 0;
		for (int i = 0; i < size; ++i) {
			v = (b[i] - a[i]) / stdd;
			d += (v * v);
		}
		if (std::isnan(d) || std::isinf(d)) return 0;
		else return std::exp(-d);
	}

	template <typename T>
	T cosineSimilarity (const T* a, const T* b, int size) {
		// NB: it assumes vector have the same size
		int dotProd = 0;
		T lenA = 0;
		T lenB = 0;

		for (int i = 0; i < size; ++i) {
			dotProd += (a[i] * b[i]);
			lenA += (a[i] * a[i]);
			lenB += (b[i] * b[i]);
		}

		T den = std::sqrt(lenA * lenB);
		if (std::isnan(den) || std::isinf(den)) return 0;
		return den != 0 ? ((T) dotProd / den) : 0;
	}

	template <typename T>
	T taxicab (const T* a, const T* b, int size) {
		// NB: it assumes vector have the same size
		T sum = 0;

		for (int i = 0; i < size; ++i) {
			T s = a[i] - b[i];
			sum += fabs(s);
		}
		if (std::isnan(sum) || std::isinf(sum)) return 0;
		else return sum;
	}

	template <typename T>
	T kullbackLeibler (const T* a, const T* b, int size){
		T d = 0;
		for (int i = 0; i < size; ++i){
			if (b[i] != 0) {
				d += a[i] * log (a[i] / b[i]);
			}
			else {
				d += a[i] * log (a[i] / EPS);
			}
		}
		if (std::isnan(d) || std::isinf(d)) return 0;
		else return d;
	}

	// ------------------------------------------------------------------//

	class Matrix {
		std::valarray<Real> data;
		int dim;
	public:
		Matrix(int r, int c) : data(r*c), dim(c) {}
		Real& operator()(int r, int c) { return data[r * dim + c]; }
		int trace() const { return data[std::slice(0, dim, dim + 1)].sum(); }
	};

	template <typename T>
	void covmat(T** data, int n, int m, T** symmat) {
		T* mean = new T[m];

		for (int j = 0; j < m; j++) {
			mean[j] = 0.0;
			for (int i = 0; i < n; i++) {
				mean[j] += data[i][j];
			}
			mean[j] /= (T) n;
		}

		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				data[i][j] -= mean[j];
			}
		}

		for (int j1 = 0; j1 < m; j1++) {
			for (int j2 = j1; j2 < m; j2++) {
				symmat[j1][j2] = 0.0;
				for (int i = 0; i < n; i++) {
					symmat[j1][j2] += data[i][j1] * data[i][j2];
				}
				symmat[j2][j1] = symmat[j1][j2];
			}
		}

		delete [] mean;
	}

	template <typename T>
	void tred2(T** a, int n, T* d, T* e) {
		int l, k, j, i;
		T scale, hh, h, g, f;

		for (i = 0; i < n; ++i) (a[i])--;
		a--;
		d--;
		e--;

		for (i = n; i >= 2; i--) {
			l = i - 1;
			h = scale = 0.0;
			if (l > 1) {
				for (k = 1; k <= l; k++)
					scale += fabs(a[i][k]);
				if (scale == 0.0)
					e[i] = a[i][l];
				else {
					for (k = 1; k <= l; k++) {
						a[i][k] /= scale;
						h += a[i][k] * a[i][k];
					}
					f = a[i][l];
					g = f > 0 ? -sqrt(h) : sqrt(h);
					e[i] = scale * g;
					h -= f * g;
					a[i][l] = f - g;
					f = 0.0;
					for (j = 1; j <= l; j++) {
						a[j][i] = a[i][j] / h;
						g = 0.0;
						for (k = 1; k <= j; k++)
							g += a[j][k] * a[i][k];
						for (k = j + 1; k <= l; k++)
							g += a[k][j] * a[i][k];
						e[j] = g / h;
						f += e[j] * a[i][j];
					}
					hh = f / (h + h);
					for (j = 1; j <= l; j++) {
						f = a[i][j];
						e[j] = g = e[j] - hh * f;
						for (k = 1; k <= j; k++)
							a[j][k] -= (f * e[k] + g * a[i][k]);
					}
				}
			} else
				e[i] = a[i][l];
			d[i] = h;
		}
		d[1] = 0.0;
		e[1] = 0.0;
		for (i = 1; i <= n; i++) {
			l = i - 1;
			if (d[i]) {
				for (j = 1; j <= l; j++) {
					g = 0.0;
					for (k = 1; k <= l; k++)
						g += a[i][k] * a[k][j];
					for (k = 1; k <= l; k++)
						a[k][j] -= g * a[k][i];
				}
			}
			d[i] = a[i][i];
			a[i][i] = 1.0;
			for (j = 1; j <= l; j++)
				a[j][i] = a[i][j] = 0.0;
		}

		a++;
		d++;
		e++;
		for (i = 0; i < n; ++i) (a[i])++;
	}

	template <typename T>
	void tqli(T d[], T e[], int n, T** z) {
		int m, l, iter, i, k;
		float s, r, p, g, f, dd, c, b;

		for (i = 0; i < n; ++i) (z[i])--;
		z--;
		d--;
		e--;

		for (i = 2; i <= n; i++)
			e[i - 1] = e[i];
		e[n] = 0.0;
		for (l = 1; l <= n; l++) {
			iter = 0;
			do {
				for (m = l; m <= n - 1; m++) {
					dd = fabs(d[m]) + fabs(d[m + 1]);
					if (fabs(e[m]) + dd == dd) break;
				}
				if (m != l) {
					if (iter++ == 30) throw std::runtime_error("no convergence in TLQI");
					g = (d[l + 1] - d[l]) / (2.0 * e[l]);
					r = sqrt((g * g) + 1.0);
					g = d[m] - d[l] + e[l] / (g + PCASIGN(r, g));
					s = c = 1.0;
					p = 0.0;
					for (i = m - 1; i >= l; i--) {
						f = s * e[i];
						b = c * e[i];
						if (fabs(f) >= fabs(g)) {
							c = g / f;
							r = sqrt((c * c) + 1.0);
							e[i + 1] = f * r;
							c *= (s = 1.0 / r);
						} else {
							s = f / g;
							r = sqrt((s * s) + 1.0);
							e[i + 1] = g * r;
							s *= (c = 1.0 / r);
						}
						g = d[i + 1] - p;
						r = (d[i] - g) * s + 2.0 * c * b;
						p = s * r;
						d[i + 1] = g + p;
						g = c * r - b;
						for (k = 1; k <= n; k++) {
							f = z[k][i + 1];
							z[k][i + 1] = s * z[k][i] + c * f;
							z[k][i] = c * z[k][i] - s * f;
						}
					}
					d[l] = d[l] - p;
					e[l] = g;
					e[m] = 0.0;
				}
			} while (m != l);
		}

		z++;
		d++;
		e++;
		for (i = 0; i < n; ++i) (z[i])++;
	}

	// ------------------------------------------------------------------//

	template <typename T>
	void kmeans(T** data, int n, int m, int k, T t, int* labels, T** centroids) {
		int h, i, j;
		T old_error, error = DBL_MAX;
		T **c = centroids;
		T **c1 = (T**) new T*[k];
		int* counts = (int*) new int[k];

		assert(data && k > 0 && k <= n && m > 0 && t >= 0);

		for (h = i = 0; i < k; h += n / k, i++) {
			c1[i] = (T*) calloc(m, sizeof (T));
			if (!centroids) {
				c[i] = (T*) new T[m];
			}
			for (j = m; j-- > 0; c[i][j] = data[h][j]);
		}

		do {
			old_error = error, error = 0;

			for (i = 0; i < k; counts[i++] = 0) {
				for (j = 0; j < m; c1[i][j++] = 0);
			}

			for (h = 0; h < n; h++) {
				T min_distance = DBL_MAX;
				for (i = 0; i < k; i++) {
					T distance = 0;
					for (j = m; j-- > 0; distance += pow(data[h][j] - c[i][j], 2));
					if (distance < min_distance) {
						labels[h] = i;
						min_distance = distance;
					}
				}

				for (j = m; j-- > 0; c1[labels[h]][j] += data[h][j]);
				counts[labels[h]]++;
				error += min_distance;
			}

			for (i = 0; i < k; i++) {
				for (j = 0; j < m; j++) {
					c[i][j] = counts[i] ? c1[i][j] / counts[i] : c1[i][j];
				}
			}

		} while (fabs(error - old_error) > t);


		for (i = 0; i < k; i++) {
			delete [] c1[i];
		}

		delete [] c1;
		delete [] counts;
	}
}

#endif	// ALGORITHMS_H

// EOF