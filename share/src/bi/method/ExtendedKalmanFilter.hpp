/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev$
 * $Date$
 */
#ifndef BI_METHOD_EXTENDEDKALMANFILTER_HPP
#define BI_METHOD_EXTENDEDKALMANFILTER_HPP

#include "Simulator.hpp"
#include "Observer.hpp"
#include "misc.hpp"
#include "../misc/location.hpp"
#include "../misc/exception.hpp"

namespace bi {
/**
 * Extended Kalman filter.
 *
 * @ingroup method
 *
 * @tparam B Model type.
 * @tparam S Simulator type.
 * @tparam IO1 Output type.
 *
 * @section Concepts
 *
 * #concept::Filter
 */
template<class B, class S, class IO1>
class ExtendedKalmanFilter {
public:
  /**
   * Constructor.
   *
   * @param m Model.
   * @param sim Simulator.
   * @param out Output.
   */
  ExtendedKalmanFilter(B& m, S* sim = NULL, IO1* out = NULL);

  /**
   * @name High-level interface.
   *
   * An easier interface for common usage.
   */
  //@{
  /**
   * Get simulator.
   *
   * @return Simulator.
   */
  S* getSim();

  /**
   * Set simulator.
   *
   * @param sim Simulator.
   */
  void setSim(S* sim);

  /**
   * Get output.
   *
   * @return Output.
   */
  IO1* getOutput();

  /**
   * Set output.
   *
   * @param out Output.
   */
  void setOutput(IO1* out);

  /**
   * Filter forward.
   *
   * @tparam L Location.
   * @tparam IO2 Input type.
   *
   * @param[in,out] rng Random number generator.
   * @param first Start of time schedule.
   * @param last End of time schedule.
   * @param[out] s State.
   * @param inInit Initialisation file.
   *
   * @return Estimate of the marginal log-likelihood.
   */
  template<Location L, class IO2>
  real filter(Random& rng, const ScheduleIterator first,
      const ScheduleIterator last, State<B,L>& s, IO2* inInit)
          throw (CholeskyException);

  /**
   * Filter forward, with fixed parameters.
   *
   * @tparam L Location.
   * @tparam V1 Vector type.
   *
   * @param[in,out] rng Random number generator.
   * @param first Start of time schedule.
   * @param last End of time schedule.
   * @param theta Parameters.
   * @param[out] s State.
   *
   * @return Estimate of the marginal log-likelihood.
   */
  template<Location L, class V1>
  real filter(Random& rng, const ScheduleIterator first,
      const ScheduleIterator last, const V1 theta, State<B,L>& s)
          throw (CholeskyException);

  /**
   * @copydoc #concept::Filter::sampleTrajectory()
   */
  template<class M1>
  void sampleTrajectory(Random& rng, M1 X);
  //@}

  /**
   * @name Low-level interface.
   *
   * Largely used by other features of the library or for finer control over
   * performance and behaviour.
   */
  //@{
  /**
   * Initialise.
   *
   * @tparam L Location.
   * @tparam V1 Vector type.
   * @tparam M1 Matrix type.
   * @tparam IO2 Input type.
   *
   * @param[in,out] rng Random number generator.
   * @param now Current step in time schedule.
   * @param[out] s State.
   * @param[out] mu1 Predicted mean.
   * @param[out] U1 Cholesky factor of predicted covariance matrix.
   * @param[out] C Time cross-covariance.
   * @param inInit Initialisation file.
   */
  template<Location L, class V1, class M1, class IO2>
  void init(Random& rng, const ScheduleElement now, State<B,L>& s, V1 mu1,
      M1 U1, M1 C, IO2* inInit);

  /**
   * Initialise, with fixed parameters.
   *
   * @tparam L Location.
   * @tparam V2 Vector type.
   * @tparam V1 Vector type.
   * @tparam M1 Matrix type.
   *
   * @param[in,out] rng Random number generator.
   * @param now Current step in time schedule.
   * @param theta Parameters.
   * @param s State.
   * @param[out] mu1 Predicted mean.
   * @param[out] U1 Cholesky factor of predicted covariance matrix.
   * @param[out] C Time cross-covariance.
   * @param[out] S Square-root covariance matrix.
   */
  template<Location L, class V2, class V1, class M1>
  void init(Random& rng, const ScheduleElement now, const V2 theta,
      State<B,L>& s, V1 mu1, M1 U1, M1 C);

  /**
   * Predict and correct.
   *
   * @tparam L Location.
   * @tparam V1 Vector type.
   * @tparam M1 Matrix type.
   *
   * @param[in,out] iter Current position in time schedule. Advanced on
   * return.
   * @param last End of time schedule.
   * @param[in,out] s State.
   * @param[in,out] mu1 Predicted mean.
   * @param[in,out] U1 Cholesky factor of predicted covariance matrix.
   * @param[in,out] mu2 Corrected mean.
   * @param[in,out] U2 Cholesky factor of corrected covariance matrix.
   * @param[in,out] C Time cross-covariance.
   *
   * @return Estimate of the incremental log-likelihood.
   */
  template<Location L, class V1, class M1>
  real step(ScheduleIterator& iter, const ScheduleIterator last,
      State<B,L>& s, V1 mu1, M1 U1, V1 mu2, M1 U2, M1 C)
          throw (CholeskyException);

  /**
   * Predict.
   *
   * @tparam L Location.
   * @tparam V1 Vector type.
   * @tparam M1 Matrix type.
   *
   * @param next Next step in time schedule.
   * @param[in,out] s State.
   * @param[out] mu1 Predicted mean.
   * @param[out] U1 Cholesky factor of predicted covariance matrix.
   * @param mu2 Corrected mean.
   * @param U2 Cholesky factor of corrected covariance matrix.
   * @param[in,out] C Time cross-covariance.
   */
  template<Location L, class V1, class M1>
  void predict(const ScheduleElement next, State<B,L>& s, V1 mu1, M1 U1,
      const V1 mu2, const M1 U2, M1 C) throw (CholeskyException);

  /**
   * Correct prediction with observation to produce filter density.
   *
   * @tparam L Location.
   * @tparam V1 Vector type.
   * @tparam M1 Matrix type.
   *
   * @param now Current step in time schedule.
   * @param[in,out] s State.
   * @param mu1 Predicted mean.
   * @param U1 Cholesky factor of predicted covariance matrix.
   * @param[out] U2 Cholesky factor of corrected covariance matrix.
   *
   * @return Estimate of the incremental log-likelihood.
   */
  template<Location L, class V1, class M1>
  real correct(const ScheduleElement now, State<B,L>& s, const V1 mu1,
      const M1 U1, V1 mu2, M1 U2) throw (CholeskyException);

  /**
   * Output static variables.
   *
   * @param L Location.
   *
   * @param s State.
   */
  template<Location L>
  void output0(const State<B,L>& s);

  /**
   * Output.
   *
   * @tparam L Location.
   * @tparam V1 Vector type.
   * @tparam M1 Matrix type.
   *
   * @param now Current step in time schedule.
   * @param s State.
   * @param mu1 Predicted mean.
   * @param U1 Cholesky factor of predicted covariance matrix.
   * @param mu2 Corrected mean.
   * @param U2 Cholesky factor of corrected covariance matrix.
   * @param C Time cross-covariance.
   */
  template<Location L, class V1, class M1>
  void output(const ScheduleElement now, const State<B,L>& s, const V1 mu1,
      const M1 U1, const V1 mu2, const M1 U2, const M1 C);

  /**
   * Output marginal log-likelihood estimate.
   *
   * @param ll Estimate of the marginal log-likelihood.
   */
  void outputT(const real ll);

  /**
   * Clean up.
   */
  void term();
  //@}

protected:
  /**
   * Model.
   */
  B& m;

  /**
   * Simulator.
   */
  S* sim;

  /**
   * Output.
   */
  IO1* out;

  /*
   * Sizes for convenience.
   */
  static const int NR = B::NR;
  static const int ND = B::ND;
  static const int NO = B::NO;
  static const int M = NR + ND;
};

/**
 * Factory for creating ExtendedKalmanFilter objects.
 *
 * @ingroup method
 *
 * @see ExtendedKalmanFilter
 */
struct ExtendedKalmanFilterFactory {
  /**
   * Create extended Kalman filter.
   *
   * @return ExtendedKalmanFilter object. Caller has ownership.
   *
   * @see ExtendedKalmanFilter::ExtendedKalmanFilter()
   */
  template<class B, class S, class IO1>
  static ExtendedKalmanFilter<B,S,IO1>* create(B& m, S* sim = NULL, IO1* out =
      NULL) {
    return new ExtendedKalmanFilter<B,S,IO1>(m, sim, out);
  }
};
}

#include "../math/view.hpp"
#include "../math/operation.hpp"
#include "../math/operation.hpp"
#include "../math/pi.hpp"
#include "../math/loc_temp_vector.hpp"
#include "../math/loc_temp_matrix.hpp"

template<class B, class S, class IO1>
bi::ExtendedKalmanFilter<B,S,IO1>::ExtendedKalmanFilter(B& m, S* sim,
    IO1* out) :
    m(m), sim(sim), out(out) {
  //
}

template<class B, class S, class IO1>
inline S* bi::ExtendedKalmanFilter<B,S,IO1>::getSim() {
  return sim;
}

template<class B, class S, class IO1>
inline void bi::ExtendedKalmanFilter<B,S,IO1>::setSim(S* sim) {
  this->sim = sim;
}

template<class B, class S, class IO1>
inline IO1* bi::ExtendedKalmanFilter<B,S,IO1>::getOutput() {
  return out;
}

template<class B, class S, class IO1>
inline void bi::ExtendedKalmanFilter<B,S,IO1>::setOutput(IO1* out) {
  this->out = out;
}

template<class B, class S, class IO1>
template<bi::Location L, class IO2>
real bi::ExtendedKalmanFilter<B,S,IO1>::filter(Random& rng,
    const ScheduleIterator first, const ScheduleIterator last, State<B,L>& s,
    IO2* inInit) throw (CholeskyException) {
  typedef typename loc_temp_vector<L,real>::type vector_type;
  typedef typename loc_temp_matrix<L,real>::type matrix_type;

  vector_type mu1(M), mu2(M);
  matrix_type U1(M, M), U2(M, M), C(M, M);
  real ll = 0.0;

  ScheduleIterator iter = first;
  init(rng, *iter, s, mu1, U1, C, inInit);
  output0(s);
  ll = correct(*iter, s, mu1, U1, mu2, U2);
  output(*iter, s, mu1, U1, mu2, U2, C);
  while (iter + 1 != last) {
    ll += step(iter, last, s, mu1, U1, mu2, U2, C);
  }
  term();
  outputT(ll);

  return ll;
}

template<class B, class S, class IO1>
template<bi::Location L, class V1>
real bi::ExtendedKalmanFilter<B,S,IO1>::filter(Random& rng,
    const ScheduleIterator first, const ScheduleIterator last, const V1 theta,
    State<B,L>& s) throw (CholeskyException) {
  typedef typename loc_temp_vector<L,real>::type vector_type;
  typedef typename loc_temp_matrix<L,real>::type matrix_type;

  vector_type mu1(M), mu2(M);
  matrix_type U1(M, M), U2(M, M), C(M, M);
  real ll = 0.0;

  ScheduleIterator iter = first;
  init(rng, *iter, theta, s, mu1, U1, C);
  output0(s);
  ll = correct(*iter, s, mu1, U1, mu2, U2);
  output(*iter, s, mu1, U1, mu2, U2, C);
  while (iter + 1 != last) {
    ll += step(iter, last, s, mu1, U1, mu2, U2, C);
  }
  term();
  outputT(ll);

  return ll;
}

template<class B, class S, class IO1>
template<class M1>
void bi::ExtendedKalmanFilter<B,S,IO1>::sampleTrajectory(Random& rng, M1 X) {
  typedef typename sim_temp_vector<M1>::type vector_type;
  typedef typename sim_temp_matrix<M1>::type matrix_type;

  matrix_type U1(M, M), U2(M, M), C(M, M);
  vector_type mu1(M), mu2(M);

  int k = out->size();
  try {
    while (k > 0) {
      out->readCorrectedMean(k - 1, mu1);
      out->readCorrectedStd(k - 1, U1);

      if (k < out->size()) {
        out->readPredictedMean(k, mu2);
        out->readPredictedStd(k, U2);
        out->readCross(k, C);

        condition(mu1, U1, mu2, U2, C, column(X, k));
      }

      rng.gaussians(column(X, k - 1));
      trmv(U1, column(X, k - 1));
      axpy(1.0, mu1, column(X, k - 1));

      --k;
    }
  } catch (CholeskyException e) {
    BI_WARN_MSG(false, "Cholesky factorisation exception sampling trajectory");
  }
}

template<class B, class S, class IO1>
template<bi::Location L, class V1, class M1, class IO2>
void bi::ExtendedKalmanFilter<B,S,IO1>::init(Random& rng,
    const ScheduleElement now, State<B,L>& s, V1 mu1, M1 U1, M1 C,
    IO2* inInit) {
  typedef typename loc_temp_matrix<L,real>::type matrix_type;

  BOOST_AUTO(F, reshape(s.template getVar<VarGroupF>(), ND + NR, ND + NR));
  BOOST_AUTO(Q, reshape(s.template getVar<VarGroupQ>(), ND + NR, ND + NR));
  BOOST_AUTO(G, reshape(s.template getVar<VarGroupG>(), ND + NR, NO));
  BOOST_AUTO(R, reshape(s.template getVar<VarGroupR>(), NO, NO));

  ident(F);
  Q.clear();
  G.clear();
  R.clear();

  /* initialise */
  sim->init(rng, now, s, inInit);

  /* predicted mean */
  mu1 = row(s.getDyn(), 0);

  /* Cholesky factor of predicted covariance */
  U1 = Q;
  subrange(U1, 0, NR, NR, ND) = subrange(F, 0, NR, NR, ND);
  trmm(1.0, subrange(U1, 0, NR, 0, NR), subrange(U1, 0, NR, NR, ND));

  /* across-time covariance */
  C.clear();

  /* within-time covariance */
  //matrix_type Sigma(M, M);
  //Sigma.clear();
  //syrk(1.0, U, 0.0, Sigma, 'U', 'T');
  /* recover Cholesky factor from within-time covariance matrix */
  //chol(Sigma, U);
  if (out != NULL) {
    out->clear();
  }
}

template<class B, class S, class IO1>
template<bi::Location L, class V2, class V1, class M1>
void bi::ExtendedKalmanFilter<B,S,IO1>::init(Random& rng,
    const ScheduleElement now, const V2 theta, State<B,L>& s, V1 mu1, M1 U1,
    M1 C) {
  // this should be the same as init() above, but with a different call to
  // sim->init()

  typedef typename loc_temp_matrix<L,real>::type matrix_type;

  BOOST_AUTO(F, reshape(s.template getVar<VarGroupF>(), ND + NR, ND + NR));
  BOOST_AUTO(Q, reshape(s.template getVar<VarGroupQ>(), ND + NR, ND + NR));
  BOOST_AUTO(G, reshape(s.template getVar<VarGroupG>(), ND + NR, NO));
  BOOST_AUTO(R, reshape(s.template getVar<VarGroupR>(), NO, NO));

  ident(F);
  Q.clear();
  G.clear();
  R.clear();

  /* initialise */
  sim->init(rng, theta, now, s);

  /* predicted mean */
  mu1 = row(s.getDyn(), 0);

  /* Cholesky factor of predicted covariance */
  U1 = Q;
  subrange(U1, 0, NR, NR, ND) = subrange(F, 0, NR, NR, ND);
  trmm(1.0, subrange(U1, 0, NR, 0, NR), subrange(U1, 0, NR, NR, ND));

  /* across-time covariance */
  C.clear();

  /* within-time covariance */
  //matrix_type Sigma(M, M);
  //Sigma.clear();
  //syrk(1.0, U, 0.0, Sigma, 'U', 'T');
  /* recover Cholesky factor from within-time covariance matrix */
  //chol(Sigma, U);
  if (out != NULL) {
    out->clear();
  }
}

template<class B, class S, class IO1>
template<bi::Location L, class V1, class M1>
real bi::ExtendedKalmanFilter<B,S,IO1>::step(ScheduleIterator& iter,
    const ScheduleIterator last, State<B,L>& s, V1 mu1, M1 U1, V1 mu2, M1 U2,
    M1 C) throw (CholeskyException) {
  do {
    ++iter;
    predict(*iter, s, mu1, U1, mu2, U2, C);
  } while (iter + 1 != last && !iter->hasOutput());
  real ll = correct(*iter, s, mu1, U1, mu2, U2);
  output(*iter, s, mu1, U1, mu2, U2, C);

  return ll;
}

template<class B, class S, class IO1>
template<bi::Location L, class V1, class M1>
void bi::ExtendedKalmanFilter<B,S,IO1>::predict(const ScheduleElement next,
    State<B,L>& s, V1 mu1, M1 U1, const V1 mu2, const M1 U2, M1 C)
        throw (CholeskyException) {
  typedef typename loc_temp_matrix<L,real>::type matrix_type;

  BOOST_AUTO(F, reshape(s.template getVar<VarGroupF>(), ND + NR, ND + NR));
  BOOST_AUTO(Q, reshape(s.template getVar<VarGroupQ>(), ND + NR, ND + NR));

  /* predict */
  sim->advance(next, s);

  /* predicted mean */
  mu1 = row(s.getDyn(), 0);

  /* across-time block of square-root covariance */
  columns(C, 0, NR).clear();
  subrange(C, 0, NR, NR, ND).clear();
  subrange(C, NR, ND, NR, ND) = subrange(F, NR, ND, NR, ND);
  trmm(1.0, U2, C);

  /* current-time block of square-root covariance */
  rows(U1, NR, ND).clear();
  subrange(U1, 0, NR, 0, NR) = subrange(Q, 0, NR, 0, NR);
  subrange(U1, 0, NR, NR, ND) = subrange(F, 0, NR, NR, ND);
  trmm(1.0, subrange(U1, 0, NR, 0, NR), subrange(U1, 0, NR, NR, ND));

  /* predicted covariance */
  matrix_type Sigma(M, M);
  Sigma.clear();
  syrk(1.0, C, 0.0, Sigma, 'U', 'T');
  syrk(1.0, U1, 1.0, Sigma, 'U', 'T');

  /* across-time covariance */
  trmm(1.0, U2, C, 'L', 'U', 'T');

  /* Cholesky factor of predicted covariance */
  chol(Sigma, U1);

  /* reset Jacobian, as it has now been multiplied in */
  ident(F);
  Q.clear();
}

template<class B, class S, class IO1>
template<bi::Location L, class V1, class M1>
real bi::ExtendedKalmanFilter<B,S,IO1>::correct(const ScheduleElement now,
    State<B,L>& s, const V1 mu1, const M1 U1, V1 mu2, M1 U2)
        throw (CholeskyException) {
  typedef typename loc_temp_matrix<L,real>::type matrix_type;
  typedef typename loc_temp_vector<L,real>::type vector_type;
  typedef typename loc_temp_vector<L,int>::type int_vector_type;

  BOOST_AUTO(G, reshape(s.template getVar<VarGroupG>(), ND + NR, NO));
  BOOST_AUTO(R, reshape(s.template getVar<VarGroupR>(), NO, NO));

  real ll = 0.0;
  mu2 = mu1;
  U2 = U1;

  if (now.hasObs()) {
    BOOST_AUTO(mask, sim->getObs()->getMask(now.indexObs()));
    const int W = mask.size();

    sim->observe(s);

    matrix_type C(M, W), U3(W, W), Sigma3(W, W), R3(W, W);
    vector_type y(W), z(W), mu3(W);
    int_vector_type map(W);

    /* construct projection from mask */
    Var* var;
    int id, start = 0, size;
    for (id = 0; id < m.getNumVars(O_VAR); ++id) {
      var = m.getVar(O_VAR, id);
      size = mask.getSize(id);

      if (mask.isSparse(id)) {
        addscal_elements(mask.getIndices(id), var->getStart(),
            subrange(map, start, size));
      } else {
        seq_elements(subrange(map, start, size), var->getStart());
      }
      start += size;
    }

    /* project matrices and vectors to active variables in mask */
    gather_columns(map, G, C);
    gather_matrix(map, map, R, R3);
    gather(map, row(s.get(O_VAR), 0), mu3);
    gather(map, row(s.get(OY_VAR), 0), y);

    trmm(1.0, U1, C);

    Sigma3.clear();
    syrk(1.0, C, 0.0, Sigma3, 'U', 'T');
    syrk(1.0, R3, 1.0, Sigma3, 'U', 'T');
    trmm(1.0, U1, C, 'L', 'U', 'T');
    chol(Sigma3, U3, 'U');

    /* incremental log-likelihood */
    ///@todo Duplicates some operations in condition() calls below
    sub_elements(y, mu3, z);
    trsv(U3, z, 'U');
    ll = -0.5 * dot(z) - BI_HALF_LOG_TWO_PI
        - bi::log(prod_reduce(diagonal(U3)));

    if (now.indexTime() > 0) {
      condition(mu2, U2, mu3, U3, C, y);
    } else {
      condition(subrange(mu2, NR, ND), subrange(U2, NR, ND, NR, ND), mu3, U3,
          rows(C, NR, ND), y);
    }
    row(s.getDyn(), 0) = mu2;

    /* reset Jacobian */
    G.clear();
    R.clear();
  }

  return ll;
}

template<class B, class S, class IO1>
template<bi::Location L>
void bi::ExtendedKalmanFilter<B,S,IO1>::output0(const State<B,L>& s) {
  if (out != NULL) {
    //out->writeParameters(s);
  }
}

template<class B, class S, class IO1>
template<bi::Location L, class V1, class M1>
void bi::ExtendedKalmanFilter<B,S,IO1>::output(const ScheduleElement now,
    const State<B,L>& s, const V1 mu1, const M1 U1, const V1 mu2, const M1 U2,
    const M1 C) {
  if (out != NULL && now.hasOutput()) {
    const int k = now.indexOutput();

    out->writeTime(k, now.getTime());
    out->writeState(k, s);
    out->writePredictedMean(k, mu1);
    out->writePredictedStd(k, U1);
    out->writeCorrectedMean(k, mu2);
    out->writeCorrectedStd(k, U2);
    out->writeCross(k, C);
  }
}

template<class B, class S, class IO1>
void bi::ExtendedKalmanFilter<B,S,IO1>::outputT(const real ll) {
  if (out != NULL) {
    out->writeLL(ll);
  }
}

template<class B, class S, class IO1>
void bi::ExtendedKalmanFilter<B,S,IO1>::term() {
  sim->term();
}

#endif
