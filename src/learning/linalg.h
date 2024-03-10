// linalg.h

#ifndef LINALG_H
#define LINALG_H

#include <vector>
#include <cassert>
#include <cmath>
#include <valarray>
#include <cstdlib>

namespace f8 {
	// ------------------------------------------------------------------//
    template<typename T>
    class Matrix {
        int _rows;
        int _cols;
        std::valarray<T> _data;   
    public:
        Matrix (int r, int c) : _rows (r), _cols(c), _data(r * c){
            null ();
        }
        Matrix (T* d, int r, int c) : _rows (r), _cols(c), _data(d, r * c){}
        int rows () const { return _rows; }
        int cols () const { return _cols; }        
        T& operator()(int r, int c) { return _data[r * _cols + c]; }
        std::valarray<T>& data () { return _data; }
        const std::valarray<T>& data () const { return _data; }
        void null () {
            for (unsigned i = 0; i < _data.size (); ++i) _data[i] = 0;
        }
        void id () {
            if (_rows != _cols) throw std::runtime_error ("identity matrices must be square");
            null ();
            for (unsigned i = 0; i < _rows; ++i) (*this)(i, i) = 1;
        }
        T trace() const { return _data[std::slice(0, _cols, _cols + 1)].sum(); }
        Matrix<T> operator~ () {
            Matrix<T> temp (cols (), rows ());
            
            for (unsigned int i = 0; i < rows (); ++i)
                for (unsigned int j = 0; j < cols (); ++j) {
                    T x = (*this)(i, j);
                    temp (j, i) = x;
                }
            return temp;
        }        
    };    
    template<class T>
    Matrix<T> matmul (const Matrix<T>& a, const Matrix<T>& b) {
        int acols = a.cols ();
        int arows = a.rows ();
        int brows = b.rows ();
        int bcols = b.cols ();
        Matrix<T> result (arows, bcols);
        for(size_t i = 0; i < arows; ++i) {
            for(size_t j = 0; j < bcols; ++j) {
                std::valarray<T> row = a.data()[std::slice(acols * i, acols, 1)];
                std::valarray<T> col = b.data()[std::slice(j, brows, bcols)];
                result.data()[i * bcols + j] = (row * col).sum ();
            }
        } 
        return result;
    }
}

#endif // LINALG_H
