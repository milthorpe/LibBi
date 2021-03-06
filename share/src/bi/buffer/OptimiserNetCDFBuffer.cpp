/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev$
 * $Date$
 */
#include "OptimiserNetCDFBuffer.hpp"

#include "../math/view.hpp"

bi::OptimiserNetCDFBuffer::OptimiserNetCDFBuffer(const Model& m,
    const std::string& file, const FileMode mode) :
    NetCDFBuffer(file, mode), m(m), vars(NUM_VAR_TYPES) {
  if (mode == NEW || mode == REPLACE) {
    create();
  } else {
    map();
  }
}

void bi::OptimiserNetCDFBuffer::create() {
  int id, i;
  VarType type;
  Var* var;
  Dim* dim;

  /* record dimension */
  nsDim = createDim("ns");
  for (i = 0; i < m.getNumDims(); ++i) {
    dim = m.getDim(i);
    nDims.push_back(createDim(dim->getName().c_str(), dim->getSize()));
  }

  /* function value variable */
  valueVar = ncFile->add_var("optimiser.value", netcdf_real, nsDim);
  BI_ERROR_MSG(valueVar != NULL && valueVar->is_valid(),
      "Could not create optimiser.value variable");

  /* size variable */
  sizeVar = ncFile->add_var("optimiser.size", netcdf_real, nsDim);
  BI_ERROR_MSG(sizeVar != NULL && sizeVar->is_valid(),
      "Could not create optimiser.size variable");

  /* other variables */
  for (i = 0; i < NUM_VAR_TYPES; ++i) {
    type = static_cast<VarType>(i);
    vars[type].resize(m.getNumVars(type), NULL);
    if (type == P_VAR) {
      for (id = 0; id < (int)vars[type].size(); ++id) {
        var = m.getVar(type, id);
        if (var->hasOutput()) {
          vars[type][id] = createVar(var, false, false);
        }
      }
    }
  }
}

void bi::OptimiserNetCDFBuffer::map() {
  std::string name;
  int id, i;
  VarType type;
  Var* var;
  Dim* dim;

  /* dimensions */
  BI_ERROR_MSG(hasDim("ns"), "File must have ns dimension");
  nsDim = mapDim("ns");
  BI_WARN_MSG(nsDim->is_unlimited(), "ns dimension should be unlimited");
  for (i = 0; i < m.getNumDims(); ++i) {
    dim = m.getDim(i);
    BI_ERROR_MSG(hasDim(dim->getName().c_str()), "File must have " <<
        dim->getName() << " dimension");
    nDims.push_back(mapDim(dim->getName().c_str(), dim->getSize()));
  }

  /* function value variable */
  valueVar = ncFile->get_var("optimiser.value");
  BI_ERROR_MSG(valueVar != NULL && valueVar->is_valid(),
      "File does not contain variable optimiser.value");
  BI_ERROR_MSG(valueVar->num_dims() == 1, "Variable optimiser.value has " <<
      valueVar->num_dims() << " dimensions, should have 1");
  BI_ERROR_MSG(valueVar->get_dim(0) == nsDim,
      "Dimension 0 of variable optimiser.value should be ns");

  /* size variable */
  sizeVar = ncFile->get_var("optimiser.size");
  BI_ERROR_MSG(sizeVar != NULL && sizeVar->is_valid(),
      "File does not contain variable optimiser.size");
  BI_ERROR_MSG(sizeVar->num_dims() == 1, "Variable optimiser.size has " <<
      sizeVar->num_dims() << " dimensions, should have 1");
  BI_ERROR_MSG(sizeVar->get_dim(0) == nsDim,
      "Dimension 0 of variable optimiser.size should be ns");

  /* other variables */
  for (i = 0; i < NUM_VAR_TYPES; ++i) {
    type = static_cast<VarType>(i);
    if (type == P_VAR) {
      vars[type].resize(m.getNumVars(type), NULL);
      for (id = 0; id < m.getNumVars(type); ++id) {
        var = m.getVar(type, id);
        if (hasVar(var->getOutputName().c_str())) {
          vars[type][id] = mapVar(m.getVar(type, id));
        }
      }
    }
  }
}

void bi::OptimiserNetCDFBuffer::readValue(const int k, real& x) {
  /* pre-condition */
  BI_ASSERT(k >= 0 && k < nsDim->size());

  BI_UNUSED NcBool ret;
  ret = valueVar->set_cur(k);
  BI_ASSERT_MSG(ret, "Indexing out of bounds reading " << valueVar->name());
  ret = valueVar->get(&x, 1);
  BI_ASSERT_MSG(ret, "Inconvertible type reading " << valueVar->name());
}

void bi::OptimiserNetCDFBuffer::writeValue(const int k, const real& x) {
  /* pre-condition */
  BI_ASSERT(k >= 0 && k < nsDim->size());

  BI_UNUSED NcBool ret;
  ret = valueVar->set_cur(k);
  BI_ASSERT_MSG(ret, "Indexing out of bounds writing " << valueVar->name());
  ret = valueVar->put(&x, 1);
  BI_ASSERT_MSG(ret, "Inconvertible type writing " << valueVar->name());
}

void bi::OptimiserNetCDFBuffer::readSize(const int k, real& x) {
  /* pre-condition */
  BI_ASSERT(k >= 0 && k < nsDim->size());

  BI_UNUSED NcBool ret;
  ret = sizeVar->set_cur(k);
  BI_ASSERT_MSG(ret, "Indexing out of bounds reading " << sizeVar->name());
  ret = sizeVar->get(&x, 1);
  BI_ASSERT_MSG(ret, "Inconvertible type reading " << sizeVar->name());
}

void bi::OptimiserNetCDFBuffer::writeSize(const int k, const real& x) {
  /* pre-condition */
  BI_ASSERT(k >= 0 && k < nsDim->size());

  BI_UNUSED NcBool ret;
  ret = sizeVar->set_cur(k);
  BI_ASSERT_MSG(ret, "Indexing out of bounds writing " << sizeVar->name());
  ret = sizeVar->put(&x, 1);
  BI_ASSERT_MSG(ret, "Inconvertible type writing " << sizeVar->name());
}
