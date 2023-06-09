﻿// load_opencl_tool.
#include <iostream>
#include <windows.h>

#include "maths_opencl_tool.h"

using namespace std;
using namespace CRLOG_CONS;

void COUNT_TIME::Timing_Start() {
	QueryPerformanceFrequency(&litmp); // 获得时钟频率.
	frequency = (mocl_fp64)litmp.QuadPart;

	QueryPerformanceCounter(&litmp);
	_tvalue[0] = litmp.QuadPart;
}
// Time End.
mocl_fp64 COUNT_TIME::Timing_Ended() {
	QueryPerformanceCounter(&litmp);

	_tvalue[1] = litmp.QuadPart;
	dfm = (mocl_fp64)(_tvalue[1] - _tvalue[0]);
	returntime = dfm / frequency * 1000.0;

	return returntime;
}

/*
@ Matrix data processing.
maths_opencl_tool => mcore_opencl
*/

mocl_bit __mocl_matrixrgb_nullptr(mocl_MatrixImgRGB in) {
	if ((in.dataR != nullptr) && (in.dataG != nullptr) && (in.dataB != nullptr)) return false;
	else return true;
}

// ######################## Matrix Free (delete.data, length = NULL) ########################
mocl_size MOCL_Matrix1Free(mocl_Matrix1D matrix) {
	mocl_size free_size = NULL;
	if (matrix.data != nullptr) {
		// free_mem_length.
		free_size = matrix.mat_xlength;
		delete[] matrix.data;
	}
	else
		ClogOut << ClogOut.SET(CLOG_WARRING) << "MOPENCL Tool: Free Mat1.data nullptr." << CRLOGEND;

	matrix.data = nullptr;
	matrix.mat_xlength = NULL;

	return free_size;
}

mocl_size MOCL_Matrix2Free(mocl_Matrix2D matrix) {
	mocl_size free_size = NULL;
	if (matrix.data != nullptr) {
		// free_mem_length.
		free_size = matrix.mat_xlength * matrix.mat_ylength;

		for (mocl_size i = 0; i < matrix.mat_xlength; i++)
			delete[] matrix.data[i];
		delete[] matrix.data;
	}
	else
		ClogOut << ClogOut.SET(CLOG_WARRING) << "MOPENCL Tool: Free Mat2.data nullptr." << CRLOGEND;

	matrix.data = nullptr;
	matrix.mat_xlength = NULL;
	matrix.mat_ylength = NULL;

	return free_size;
}

mocl_size MOCL_Matrix3Free(mocl_Matrix3D matrix) {
	mocl_size free_size = NULL;
	if (matrix.data != nullptr) {
		// free_mem_length.
		free_size = matrix.mat_xlength * matrix.mat_ylength * matrix.mat_zlength;

		for (mocl_size i = 0; i < matrix.mat_xlength; i++)
			for (mocl_size j = 0; j < matrix.mat_ylength; j++)
				delete[] matrix.data[i][j];

		for (mocl_size i = 0; i < matrix.mat_xlength; i++)
			delete[] matrix.data[i];
		delete[] matrix.data;
	}
	else
		ClogOut << ClogOut.SET(CLOG_WARRING) << "MOPENCL Tool: Free Mat3.data nullptr." << CRLOGEND;

	matrix.data = nullptr;
	matrix.mat_xlength = NULL;
	matrix.mat_ylength = NULL;
	matrix.mat_zlength = NULL;

	return free_size;
}

mocl_size MOCL_MatrixImgFree(mocl_MatrixImgRGB matrix) {
	mocl_size free_size = NULL;
	if (!__mocl_matrixrgb_nullptr(matrix)) {
		// free_mem_length color_RGB.
		free_size = matrix.mat_width * matrix.mat_height * 3;
		for (mocl_size i = 0; i < matrix.mat_width; i++) {
			delete[] matrix.dataR[i];
			delete[] matrix.dataG[i];
			delete[] matrix.dataB[i];
		}
		delete[] matrix.dataR;
		delete[] matrix.dataG;
		delete[] matrix.dataB;

		matrix.dataR = nullptr;
		matrix.dataG = nullptr;
		matrix.dataB = nullptr;
	}
	else {
		if (matrix.dataR != nullptr) ClogOut << ClogOut.SET(CLOG_WARRING) << "MOPENCL Tool: Free MatImg.dataR nullptr." << CRLOGEND;
		if (matrix.dataG != nullptr) ClogOut << ClogOut.SET(CLOG_WARRING) << "MOPENCL Tool: Free MatImg.dataG nullptr." << CRLOGEND;
		if (matrix.dataB != nullptr) ClogOut << ClogOut.SET(CLOG_WARRING) << "MOPENCL Tool: Free MatImg.dataB nullptr." << CRLOGEND;
	}

	matrix.mat_width = NULL;
	matrix.mat_height = NULL;

	return free_size;
}

// ########################  Matrix Convert ########################
// mat2.xlen = mat3.xlen, mat2.ylen = mat3.ylen * mat3.zlen
mocl_Matrix2D MOCL_Matrix3toMatrix2(mocl_Matrix3D matrix) {
	mocl_Matrix2D returnMat = {};

	if (matrix.data != nullptr) {
		returnMat = MOCL_TOOL_NEWMATRIX1D(matrix.mat_xlength, matrix.mat_ylength * matrix.mat_zlength);

		mocl_size _count = NULL;
		for (mocl_size i = 0; i < matrix.mat_xlength; i++) {
			for (mocl_size j = 0; j < matrix.mat_ylength; j++) {
				for (mocl_size n = 0; n < matrix.mat_zlength; n++) {
					returnMat.data[i][_count] = matrix.data[i][j][n];
					_count++;
				}
			}
			_count = NULL;
		}
	}
	else
		ClogOut << ClogOut.SET(CLOG_WARRING) << "MOPENCL Tool: Mat3toMat2 in.nullptr." << CRLOGEND;
	MOCL_TOOL_FREEMATRIX3D(matrix);

	return returnMat;
}

// mat1.xlen = mat2.xlen * mat2.ylen
mocl_Matrix1D MOCL_Matrix2toMatrix1(mocl_Matrix2D matrix) {
	mocl_Matrix1D returnMat = {};

	if (matrix.data != nullptr) {
		returnMat.mat_xlength = size_t(matrix.mat_xlength * matrix.mat_ylength);
		returnMat.data = new mocl_fp32[returnMat.mat_xlength];

		mocl_size _count = NULL;
		for (mocl_size i = 0; i < matrix.mat_xlength; i++) {
			for (mocl_size j = 0; j < matrix.mat_ylength; j++) {
				returnMat.data[_count] = matrix.data[i][j];
				_count++;
			}
		}
	}
	else
		ClogOut << ClogOut.SET(CLOG_WARRING) << "MOPENCL Tool: Mat2toMat1 in.nullptr." << CRLOGEND;
	MOCL_TOOL_FREEMATRIX2D(matrix);

	return returnMat;
}

// mat1.xlen = mat2.xlen * mat2.ylen 
mocl_Matrix2D MOCL_Matrix1toMatrix2(mocl_Matrix1D matrix, mocl_size xlen, mocl_size ylen) {
	mocl_Matrix2D returnMat = {};
	mocl_size _count = NULL;

	if (matrix.data != nullptr) {
		returnMat = MOCL_TOOL_NEWMATRIX1D(xlen, ylen); 

		for (mocl_size i = 0; i < xlen; i++) {
			for (mocl_size j = 0; j < ylen; j++) {
				if (_count < matrix.mat_xlength)
					returnMat.data[i][j] = matrix.data[_count];
				else {

					ClogOut << ClogOut.SET(CLOG_ERROR) <<
						"MOPENCL Tool: Mat1toMat2 length > limit." << CRLOGEND;
					exit(-1);
				}
				_count++;
			}
		}
	}
	else
		ClogOut << ClogOut.SET(CLOG_WARRING) << "MOPENCL Tool: Mat1toMat2 in.nullptr." << CRLOGEND;

	delete[] matrix.data;
	matrix.data = nullptr;
	matrix.mat_xlength = NULL;

	return returnMat;
}

// mat3.xlen = mat2.r + mat2.g + mat2.b
mocl_Matrix3D MOCL_MatrixImgtoMatrix3(mocl_MatrixImgRGB matrix) {
	mocl_Matrix3D returnMat = {};

	if (!__mocl_matrixrgb_nullptr(matrix)) {
		returnMat = MOCL_TOOL_NEWMATRIX2D(3, matrix.mat_width, matrix.mat_height);

		for (mocl_size i = 0; i < matrix.mat_width; i++) {
			for (mocl_size j = 0; j < matrix.mat_height; j++) {
				returnMat.data[0][i][j] = matrix.dataR[i][j];
				returnMat.data[1][i][j] = matrix.dataG[i][j];
				returnMat.data[2][i][j] = matrix.dataB[i][j];
			}
		}
	}
	else
		ClogOut << ClogOut.SET(CLOG_WARRING) << "MOPENCL Tool: MatImgtoMat3 in.nullptr." << CRLOGEND;
	MOCL_TOOL_FREEMATRIXIMG(matrix);

	return returnMat;
}

// mat3.xlen = mat2.r + mat2.g + mat2.b
mocl_MatrixImgRGB MOCL_Matrix3toMatrixImg(mocl_Matrix3D matrix) {
	mocl_MatrixImgRGB returnMat = {};

	if (matrix.data != nullptr) {
		returnMat = MOCL_TOOL_NEWMATRIXIMG(matrix.mat_ylength, matrix.mat_zlength);

		for (mocl_size i = 0; i < matrix.mat_ylength; i++) {
			for (mocl_size j = 0; j < matrix.mat_zlength; j++) {
				returnMat.dataR[i][j] = matrix.data[0][i][j];
				returnMat.dataG[i][j] = matrix.data[1][i][j];
				returnMat.dataB[i][j] = matrix.data[2][i][j];
			}
		}
	}
	else
		ClogOut << ClogOut.SET(CLOG_WARRING) << "MOPENCL Tool: Mat3toMatImg in.nullptr." << CRLOGEND;
	MOCL_TOOL_FREEMATRIX3D(matrix);

	return returnMat;
}

// ########################  Matrix Create ########################
// Size = Matrix_Num * xlength * sizeof(float)
mocl_Matrix2D MOCL_CreateMatrix1(mocl_size Matrix_Num, mocl_size xlength) {
	mocl_Matrix2D returnMat = {};
	returnMat.mat_xlength = Matrix_Num;
	returnMat.mat_ylength = xlength;

	returnMat.data = new mocl_fp32 * [returnMat.mat_xlength];
	for (mocl_size i = 0; i < returnMat.mat_xlength; i++)
		returnMat.data[i] = new mocl_fp32[returnMat.mat_ylength];

	return returnMat;
}

// Size = Matrix_Num * xlength * ylength * sizeof(float)
mocl_Matrix3D MOCL_CreateMatrix2(mocl_size Matrix_Num, mocl_size xlength, mocl_size ylength) {
	mocl_Matrix3D returnMat = {};
	returnMat.mat_xlength = Matrix_Num;
	returnMat.mat_ylength = xlength;
	returnMat.mat_zlength = ylength;

	returnMat.data = new mocl_fp32 ** [returnMat.mat_xlength];
	for (mocl_size i = 0; i < returnMat.mat_xlength; i++)
		returnMat.data[i] = new mocl_fp32 * [returnMat.mat_ylength];

	for (mocl_size i = 0; i < returnMat.mat_xlength; i++)
		for (mocl_size j = 0; j < returnMat.mat_ylength; j++)
			returnMat.data[i][j] = new mocl_fp32[returnMat.mat_zlength];

	return returnMat;
}

// Size = MatrixImgRGB.mat_width * MatrixImgRGB.mat_height * 3 * sizeof(float)
mocl_MatrixImgRGB MOCL_CreateMatrixImg(mocl_size width, mocl_size height) {
	mocl_MatrixImgRGB returnMat = {};
	returnMat.mat_width = width;
	returnMat.mat_height = height;

	returnMat.dataR = new mocl_fp32 * [returnMat.mat_width];
	returnMat.dataG = new mocl_fp32 * [returnMat.mat_width];
	returnMat.dataB = new mocl_fp32 * [returnMat.mat_width];

	for (mocl_size i = 0; i < returnMat.mat_width; i++) {
		returnMat.dataR[i] = new mocl_fp32[returnMat.mat_height];
		returnMat.dataG[i] = new mocl_fp32[returnMat.mat_height];
		returnMat.dataB[i] = new mocl_fp32[returnMat.mat_height];
	}

	for (mocl_size i = 0; i < returnMat.mat_width; i++) {
		for (mocl_size j = 0; j < returnMat.mat_height; j++) {
			returnMat.dataR[i][j] = 0.0f;
			returnMat.dataG[i][j] = 0.0f;
			returnMat.dataB[i][j] = 0.0f;
		}
	}

	return returnMat;
}

// ######################## Image Matrix processing. ########################
// MartixImg x rotate.
mocl_MatrixImgRGB MOCL_MirrorRotateX(mocl_MatrixImgRGB matrix) {
	mocl_MatrixImgRGB returnMat = MOCL_TOOL_NEWMATRIXIMG(matrix.mat_width, matrix.mat_height);

	if (!__mocl_matrixrgb_nullptr(matrix)) {
		mocl_size widthCount = matrix.mat_width - 1;
		for (mocl_size i = 0; i < matrix.mat_width; i++) {
			for (mocl_size j = 0; j < matrix.mat_height; j++) {
				returnMat.dataR[i][j] = matrix.dataR[widthCount][j];
				returnMat.dataG[i][j] = matrix.dataG[widthCount][j];
				returnMat.dataB[i][j] = matrix.dataB[widthCount][j];
			}
			widthCount--;
		}
	}
	else
		ClogOut << ClogOut.SET(CLOG_WARRING) << "MOPENCL Tool: MirrorRotate.x in.nullptr." << CRLOGEND;
	MOCL_TOOL_FREEMATRIXIMG(matrix);

	return returnMat;
}

// MartixImg y rotate.
mocl_MatrixImgRGB MOCL_MirrorRotateY(mocl_MatrixImgRGB matrix) {
	mocl_MatrixImgRGB returnMat = MOCL_TOOL_NEWMATRIXIMG(matrix.mat_width, matrix.mat_height);

	if (!__mocl_matrixrgb_nullptr(matrix)) {
		mocl_size heightCount = matrix.mat_height - 1;
		for (mocl_size i = 0; i < matrix.mat_width; i++) {
			for (mocl_size j = 0; j < matrix.mat_height; j++) {
				returnMat.dataR[i][j] = matrix.dataR[heightCount][j];
				returnMat.dataG[i][j] = matrix.dataG[heightCount][j];
				returnMat.dataB[i][j] = matrix.dataB[heightCount][j];
				heightCount--;
			}
			heightCount = matrix.mat_height - 1;
		}
	}
	else
		ClogOut << ClogOut.SET(CLOG_WARRING) << "MOPENCL Tool: MirrorRotate.y in.nullptr." << CRLOGEND;
	MOCL_TOOL_FREEMATRIXIMG(matrix);

	return returnMat;
}

// MartixImg add-blend.
mocl_MatrixImgRGB MOCL_AddBlend(mocl_MatrixImgRGB matrixA, mocl_fp32 blendA,mocl_MatrixImgRGB matrixB, mocl_fp32 blendB) {
	// MatrixA +=.
	if (!__mocl_matrixrgb_nullptr(matrixA) && !__mocl_matrixrgb_nullptr(matrixB)) {
		// ImageMatrix Axy == Bxy.
		if ((matrixA.mat_width == matrixB.mat_width) && (matrixA.mat_height == matrixB.mat_height)) {
			for (mocl_size i = 0; i < matrixA.mat_width; i++) {
				for (mocl_size j = 0; j < matrixA.mat_height; j++) {
					// AddBlend = A * ba + B * bb
					matrixA.dataR[i][j] = matrixA.dataR[i][j] * blendA + matrixB.dataR[i][j] * blendB;
					matrixA.dataG[i][j] = matrixA.dataG[i][j] * blendA + matrixB.dataR[i][j] * blendB;
					matrixA.dataB[i][j] = matrixA.dataB[i][j] * blendA + matrixB.dataR[i][j] * blendB;
				}
			}
		}
		else
			ClogOut << ClogOut.SET(CLOG_WARRING) << "MOPENCL Tool: Length !=" << CRLOGEND;
	}
	else
		ClogOut << ClogOut.SET(CLOG_WARRING) << "MOPENCL Tool: AddBlend nullptr" << CRLOGEND;

	return matrixA;
}

// MartixImg cpoy.
mocl_MatrixImgRGB MOCL_CopyMatrixImg(mocl_MatrixImgRGB matrix) {
	mocl_MatrixImgRGB returnMat = {};

	if (!__mocl_matrixrgb_nullptr(matrix)) {
		returnMat = MOCL_TOOL_NEWMATRIXIMG(matrix.mat_width, matrix.mat_height);

		for (mocl_size i = 0; i < matrix.mat_width; i++) {
			for (mocl_size j = 0; j < matrix.mat_height; j++) {
				returnMat.dataR[i][j] = matrix.dataR[i][j];
				returnMat.dataG[i][j] = matrix.dataG[i][j];
				returnMat.dataB[i][j] = matrix.dataB[i][j];
			}
		}
	}
	else
		ClogOut << ClogOut.SET(CLOG_WARRING) << "MOPENCL Tool: CopyMatImg in.nullptr" << CRLOGEND;

	return returnMat;
}

// Test Function. 
// => "mcore_opencl_EXEHIGH.cpp"
// Matrix1D => Matrix3D.
void MOCLt_Matrix1DvalueToMatrix3(mocl_Matrix3D mat, mocl_size matcount, mocl_Matrix1D matin, mocl_size x, mocl_size y) {
	mocl_size _count = NULL;
	if ((mat.data != nullptr) && (matin.data != nullptr)) {
		for (mocl_size i = 0; i < x; i++) {
			for (mocl_size j = 0; j < y; j++) {
				mat.data[matcount][i][j] = matin.data[_count];
				_count++;
			}
		}
	}
	else {
		ClogOut << ClogOut.SET(CLOG_WARRING) <<
			"MOPENCL Tool: system func 'Matrix1DvalueToMatrix3'." << CRLOGEND;
		exit(-1);
	}
}