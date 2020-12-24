// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <Eigen/Core>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <vital/vital_config.h>

namespace py = pybind11;
namespace kwiver {
namespace vital  {
namespace python {

class EigenArray
{
  // We're assuming always dynamic, to make things simpler for first pass
  // TODO We can edit this later to use subclasses instead of two parallel
  Eigen::MatrixXd double_mat;
  Eigen::MatrixXf float_mat;
  char type;

  public:

    EigenArray(int rows = 2,
               int cols = 1,
               bool dynamic_rows = false,
               bool dynamic_cols = false,
               char ctype = 'd');

    void fromVectorF(std::vector< std::vector<float> >);
    void fromVectorD(std::vector< std::vector<double> >);
    static EigenArray fromArray(py::object, char);

    void setType(char ctype) { this->type = ctype; };
    char getType() { return type; };

    py::object getMatrix();
    Eigen::MatrixXd getMatrixD() { return double_mat; };
    Eigen::MatrixXf getMatrixF() { return float_mat; };
    void setMatrixF();
    void setMatrixD();

};

EigenArray::
EigenArray(int rows,
           int cols,
           VITAL_UNUSED bool dynamic_rows, // we're ignoring these, but keeping them in for API reasons
           VITAL_UNUSED bool dynamic_cols,
           char ctype)
{
  type = ctype;
  if(type == 'd') double_mat = Eigen::MatrixXd(rows, cols);
  else if(type == 'f') float_mat = Eigen::MatrixXf(rows, cols);
  else throw std::invalid_argument("Invalid matrix type. Must be 'd' or 'f'");
}

py::object
EigenArray::
getMatrix()
{
  if(this->getType() == 'd') return py::cast(&double_mat);
  else if(this->getType() == 'f') return py::cast(&float_mat);
  return py::none();
}

void
EigenArray::
fromVectorF(std::vector< std::vector<float> > data_vec)
{
  unsigned int rows = data_vec.size();
  unsigned int cols = data_vec[0].size();
  this->float_mat = Eigen::MatrixXf(rows, cols);
  for(unsigned int i = 0; i < rows; i++)
  {
    if (data_vec[i].size() != cols)
    {
       throw std::invalid_argument("Input is not an mxn matrix!");
    }
    for(unsigned int j = 0; j < cols; j++)
    {
      float_mat(i,j) = data_vec[i][j];
    }
  }
}

void
EigenArray::
fromVectorD(std::vector< std::vector<double> > data_vec)
{
  unsigned int rows = data_vec.size();
  unsigned int cols = data_vec[0].size();
  this->double_mat = Eigen::MatrixXd(rows, cols);
  for(unsigned int i = 0; i < rows; i++)
  {
    if (data_vec[i].size() != cols)
    {
       throw std::invalid_argument("Input is not an mxn matrix!");
    }
    for(unsigned int j = 0; j < cols; j++)
    {
      double_mat(i,j) = data_vec[i][j];
    }
  }
}

EigenArray
EigenArray::
fromArray(py::object data, char ctype = 'd')
{
  EigenArray retMat;

  retMat.setType(ctype);

  if(ctype == 'd')
  {
    std::vector< std::vector<double> > data_vec = data.cast<std::vector< std::vector<double> > >();
    retMat.fromVectorD(data_vec);
  }
  else if(ctype == 'f')
  {
    std::vector< std::vector<float> > data_vec = data.cast<std::vector< std::vector<float> > >();
    retMat.fromVectorF(data_vec);
  }
  else
  {
    throw std::invalid_argument("Invalid matrix type. Must be 'd' or 'f'");
  }

  return retMat;
}
}
}
}
