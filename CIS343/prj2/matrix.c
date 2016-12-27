
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "matrix.h"

/****************************************************************************
* Authors: Sam Essenburg, Isaac Smith                                       *
****************************************************************************/


/****************************************************************************
 * Creates and returns a pointer to a matrix object with the specified		*
 * number of rows and columns. The "data" field is set to a dynamically 	*
 * created array of ints of size rows * columns.							*
 *																			*
 * If the value of rows or columns is zero or negative, return NULL.		*
 ***************************************************************************/
Matrix *create(int rows, int columns)
{
    
	//Matrix *result = NULL;
    if(rows > 0 && columns > 0){

        Matrix *result = calloc(1, sizeof(Matrix));
        result->data = calloc(rows * columns, sizeof(int));

        result->rows = rows;
        result->columns = columns;

        return result;
    }
    
    Matrix *result = NULL;
    return result;
}

/****************************************************************************
 * Returns the matrix element at (row,column). Return INT_MIN (limits.h)	*
 * if either row and/or column is invalid. Row and column values start at 	*
 * zero. DO NOT modify the input matrix.									*
 ***************************************************************************/
int getValueAt(Matrix *m, int row, int column)
{
    if(row >= 0 && row <= m->rows && column >= 0 && column <= m->columns){
        int index = row*(m->columns)+column;
        return *(m->data+index);
    }
	return INT_MIN;
}

/****************************************************************************
 * If the row and column values are valid, sets the matrix element at 		*
 * (row,column) with the parameter value. Row and column values start at	*
 * zero.																	*
 ***************************************************************************/
void setValueAt(Matrix *m, int row, int column, int value)
{
    if(row >= 0 && row <= m->rows && column >= 0 && column <= m->columns){
        int index = row*(m->columns)+column;
	printf("Value: %d\n", value);
        *(m->data+index) = value;
    }
}

/****************************************************************************
 * If the input matrices are compatible, then performs matrix addition and	*
 * returns a pointer to the result matrix.									*
 * Use create(), getValueAt(), and setValueAt() functions to implement this	*
 * function. If the input matrices are not compatible, return NULL.			*
 * DO NOT modify the input matrices.										*
 ***************************************************************************/
Matrix *add(Matrix *m1, Matrix *m2)
{
  int row = m1->rows;
  int col = m1->columns;
  Matrix *result = create(row, col); 
 
  for(int i = 0; i < row; i++){
    for(int j = 0; j < col; j++){
      int index = i*(col)+j;
      int mat1 = getValueAt(m1, i, j);
      int mat2 = getValueAt(m2, i, j);
      *(result->data+index) = mat1 + mat2;
    }
  }
    return result;
}

/****************************************************************************
 * If the input matrices are compatible, then performs matrix subtraction	*
 * and returns a pointer to the result matrix.								*
 * Use create(), getValueAt(), and setValueAt() functions to implement this	*
 * function. If the input matrices are not compatible, return NULL.			*
 * DO NOT modify the input matrices.										*
 ***************************************************************************/
Matrix *subtract(Matrix *m1, Matrix *m2)
{
  int row = m1->rows;
  int col = m1->columns;
  Matrix *result = create(row, col); 
 
  for(int i = 0; i < row; i++){
    for(int j = 0; j < col; j++){
      int index = i*(col)+j;
      int mat1 = getValueAt(m1, i, j);
      int mat2 = getValueAt(m2, i, j);
      *(result->data+index) = mat1 - mat2;
    }
  }
  return result;
}

/****************************************************************************
 * Creates the transpose matrix of the input matrix and returns a pointer	*
 * to the result matrix. Use create(), getValueAt(), and setValueAt() 		*
 * functions to implement this function.									*
 * DO NOT modify the input matrix.											*
 ***************************************************************************/
Matrix *transpose(Matrix *m)
{
  //Matrix *result = NULL;

  int row = m->columns;
  int col = m->rows;
  Matrix *result = create(row, col);
	
  for(int i = 0; i < row; i++){
    for(int j = 0; j < col; j++){
      int index = i*(col)+j;
      int tran = getValueAt(m, j, i);
      *(result->data+index) = tran;  
    }
  }

	return result;
}

/****************************************************************************
 * Creates a matrix that is the product of the given scalar value and		*
 * the input matrix and returns a pointer to the result matrix.				*
 * Use create(), getValueAt(), and setValueAt() functions to implement 		*
 * this function.															*
 * DO NOT modify the input matrix.											*
 ***************************************************************************/
Matrix *scalarMultiply(Matrix *m, int scalar)
{
  int row = m->rows;
  int col = m->columns;
  Matrix *result = create(row, col); 
 
  for(int i = 0; i < row; i++){
    for(int j = 0; j < col; j++){
      int index = i*(col)+j;
      int mat = getValueAt(m, i, j);
      *(result->data+index) = mat*(scalar);
    }
  }
  return result;
}

/****************************************************************************
 * If the input matrices are compatible, then multiplies the input matrices	*
 * and returns a pointer to the result matrix. Use create(), getValueAt(), 	*
 * and setValueAt() functions to implement this function.					*														*
 *																			*
 * If the input matrices are not compatible, return NULL.					*
 * DO NOT modify the input matrices.										*											*
 ***************************************************************************/
Matrix *multiply(Matrix *m1, Matrix *m2)
{ 
    Matrix *result = NULL;
    if(m1->columns == m2->rows){

        int row = m2->columns;
        int col = m1->rows;
        result = create(row, col);
        int mult;

        for(int i = 0; i < m1->rows; i++){
          for(int j = 0; j < m2->columns; j++){
            for(int k = 0; k < m2->rows; k++){
              mult = mult + (getValueAt(m1, i, k))*(getValueAt(m2, k, j));
            }
              int index = i*(col)+j;
              *(result->data+index) = mult;
              mult = 0;        
        }
      }
   return result;
  }
    result = NULL;
    return result;
}



