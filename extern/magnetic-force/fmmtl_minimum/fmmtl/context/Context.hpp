#pragma once
/** @file Context
 *
 * A class which stores the Kernel, tree(s), and data.
 */

/** TODO: Write Context and Tree concepts */

#include <functional>

#include "fmmtl/meta/kernel_traits.hpp"
#include "fmmtl/meta/tree_traits.hpp"
#include "fmmtl/meta/range.hpp"
#include "fmmtl/tree/TreeData.hpp"

#include "fmmtl/FMMOptions.hpp"

#include "fmmtl/dispatch/S2P.hpp"
#include "fmmtl/dispatch/T2P.hpp"

namespace fmmtl {

/** @struct SingleTreeContext
 * Single tree context which aliases the source and target trees.
 */
template <typename TreeType>
class SingleTreeContext {
 public:
  typedef TreeType source_tree_type;
  typedef TreeType target_tree_type;
  FMMTL_IMPORT_TREEPAIR_TRAITS(source_tree_type, target_tree_type);

 protected:

  //! The tree of sources and targets
  source_tree_type source_tree_;

 public:
  //! Constructor
  template <typename KernelMatrix, typename Options>
  SingleTreeContext(const KernelMatrix& mat, Options& opts)
      : source_tree_(mat.sources() | transformed(S2P(mat.expansion())),
                     opts.ncrit) {
  }

  // Tree accessors
  inline source_tree_type& source_tree() {
    return source_tree_;
  }
  inline const source_tree_type& source_tree() const {
    return source_tree_;
  }
  inline target_tree_type& target_tree() {
    return source_tree_;
  }
  inline const target_tree_type& target_tree() const {
    return source_tree_;
  }
};


/** @struct DualTreeContext
 * Dual tree context with distinct source and target trees.
 */
template <typename SourceTreeType, typename TargetTreeType>
class DualTreeContext {
 public:
  typedef SourceTreeType source_tree_type;
  typedef TargetTreeType target_tree_type;
  FMMTL_IMPORT_TREEPAIR_TRAITS(source_tree_type, target_tree_type);

 protected:

  //! The tree of sources
  source_tree_type source_tree_;
  //! The tree of targets
  target_tree_type target_tree_;

 public:
  //! Constructor
  template <typename KernelMatrix, typename Options>
  DualTreeContext(const KernelMatrix& mat, Options& opts)
      : source_tree_(mat.sources() | transformed(S2P(mat.expansion())),
                     opts.ncrit),
        target_tree_(mat.targets() | transformed(T2P(mat.expansion())),
                     opts.ncrit) {
  }

  // Tree accessors
  inline source_tree_type& source_tree() {
    return source_tree_;
  }
  inline const source_tree_type& source_tree() const {
    return source_tree_;
  }
  inline target_tree_type& target_tree() {
    return target_tree_;
  }
  inline const target_tree_type& target_tree() const {
    return target_tree_;
  }
};



template <typename KernelMatrix,
          typename TreeContext>
class DataContext
    : public TreeContext {
 public:
  typedef KernelMatrix kernel_matrix_type;
  FMMTL_IMPORT_EXPANSION_TRAITS(typename kernel_matrix_type::expansion_type);

  FMMTL_IMPORT_TREEPAIR_TRAITS(typename TreeContext::source_tree_type,
                               typename TreeContext::target_tree_type);
 private:
  //! The kernel matrix this context is built for
  const kernel_matrix_type& mat_;

  // TODO: Move to Tree context?
  //! Permuted source data
  std::vector<source_type> sources_;
  //! Permuted target data
  std::vector<target_type> targets_;
  //! Permuted charge data
  std::vector<charge_type> charges_;
  //! Permuted result data
  std::vector<result_type> results_;

  std::vector<result_type> results_near_;

  //! The "multipole acceptance criteria" to decide which boxes to interact
  std::function<bool(const source_box_type&, const target_box_type&)> mac_;

  //! Multipole expansions corresponding to Box indices in Tree
  typedef std::vector<multipole_type> multipole_container;
  multipole_container M_;
  //! Local expansions corresponding to Box indices in Tree
  typedef std::vector<local_type> local_container;
  local_container L_;

 public:
  template <class Options>
  DataContext(const kernel_matrix_type& mat, Options& opts)
      : TreeContext(mat, opts),
        mat_(mat),
        // Pre-permute the sources and targets
        sources_(permute_begin(this->source_tree(), mat_.sources().begin()),
                 permute_end  (this->source_tree(), mat_.sources().begin())),
        targets_(permute_begin(this->target_tree(), mat_.targets().begin()),
                 permute_end  (this->target_tree(), mat_.targets().begin())),
        mac_(opts.MAC()),
        // TODO: only allocate if used...
        M_(this->source_tree().boxes()),
        L_(this->target_tree().boxes()) {
  }

  template <typename Executor>
  inline void execute(const std::vector<charge_type>& charges,
                      std::vector<result_type>& results,
                      Executor* exec) {
    charges_.assign(permute_begin(this->source_tree(), charges.begin()),
                    permute_end  (this->source_tree(), charges.begin()));

    results_.assign(results.size(), result_type{}-result_type{});
    results_near_.assign(results.size(), result_type{}-result_type{});
    exec->execute(*this);

    // Accumulate the permuted result
    auto pri = permute_begin(this->target_tree(), results.begin());
    auto rneari = results_near_.begin();
    for (auto ri = results_.begin(); ri != results_.end(); ++ri, ++pri, ++rneari)
      *pri = *ri+*rneari;
  }

  const expansion_type& expansion() const {
    return mat_.expansion();
  }
  const kernel_type& kernel() const {
    return mat_.kernel();
  }

  // Accessors to the multipole expansion of a source box
  inline multipole_type& multipole(const source_box_type& box) {
    return M_[box.index()];
  }
  inline const multipole_type& multipole(const source_box_type& box) const {
    return M_[box.index()];
  }
  // Accessors to the local expansion of a target box
  inline local_type& local(const target_box_type& box) {
    return L_[box.index()];
  }
  inline const local_type& local(const target_box_type& box) const {
    return L_[box.index()];
  }

  // Accept or reject the interaction of this source-target box pair
  inline bool mac(const source_box_type& sbox,
                  const target_box_type& tbox) const {
    return mac_(sbox, tbox);
  }

  // Define the body data iterators
  typedef typename std::vector<source_type>::const_iterator source_iterator;
  typedef typename std::vector<target_type>::const_iterator target_iterator;
  typedef typename std::vector<charge_type>::const_iterator charge_iterator;
  typedef typename std::vector<result_type>::iterator       result_iterator;

  // Accessor to the source data of a source body
  inline source_iterator source(const source_body_iterator& sbi) const {
    return sources_.begin() + std::distance(this->source_tree().body_begin(), sbi);
  }
  // Convenience methods for the sources
  inline source_iterator source_begin(const source_box_type& b) const {
    return this->source(b.body_begin());
  }
  inline source_iterator source_end(const source_box_type& b) const {
    return this->source(b.body_end());
  }
  inline source_iterator source_begin() const {
    return this->source(this->source_tree().body_begin());
  }
  inline source_iterator source_end() const {
    return this->source(this->source_tree().body_end());
  }

  // Accessor to the charge of a source body
  inline charge_iterator charge(const source_body_iterator& sbi) const {
    return charges_.begin() + std::distance(this->source_tree().body_begin(), sbi);
  }
  // Convenience methods for charges
  inline charge_iterator charge_begin(const source_box_type& b) const {
    return this->charge(b.body_begin());
  }
  inline charge_iterator charge_end(const source_box_type& b) const {
    return this->charge(b.body_end());
  }
  inline charge_iterator charge_begin() const {
    return this->charge(this->source_tree().body_begin());
  }
  inline charge_iterator charge_end() const {
    return this->charge(this->source_tree().body_end());
  }

  // Accessor to the target data of a target body
  inline target_iterator target(const target_body_iterator& tbi) const {
    return targets_.begin() + std::distance(this->target_tree().body_begin(), tbi);
  }
  // Convenience methods for the targets
  inline target_iterator target_begin(const target_box_type& b) const {
    return this->target(b.body_begin());
  }
  inline target_iterator target_end(const target_box_type& b) const {
    return this->target(b.body_end());
  }
  inline target_iterator target_begin() const {
    return this->target(this->target_tree().body_begin());
  }
  inline target_iterator target_end() const {
    return this->target(this->target_tree().body_end());
  }

  // Accessor to the result of a target body
  inline result_iterator result(const target_body_iterator& tbi) {
    return results_.begin() + std::distance(this->target_tree().body_begin(), tbi);
  }
  // Convenience methods for results
  inline result_iterator result_begin(const target_box_type& b) {
    return this->result(b.body_begin());
  }
  inline result_iterator result_end(const target_box_type& b) {
    return this->result(b.body_end());
  }
  inline result_iterator result_begin() {
    return this->result(this->target_tree().body_begin());
  }
  inline result_iterator result_end() {
    return this->result(this->target_tree().body_end());
  }

  //accessor to the nearbatch result;
  inline result_iterator result_near(const target_body_iterator& tbi) {
    return results_near_.begin() + std::distance(this->target_tree().body_begin(), tbi);
  }
  // Convenience methods for results
  inline result_iterator result_near_begin(const target_box_type& b) {
    return this->result_near(b.body_begin());
  }
  inline result_iterator result_near_end(const target_box_type& b) {
    return this->result_near(b.body_end());
  }
  inline result_iterator result_near_begin() {
    return this->result_near(this->target_tree().body_begin());
  }
  inline result_iterator result_near_end() {
    return this->result_near(this->target_tree().body_end());
  }
};

} // end namespace fmmtl
