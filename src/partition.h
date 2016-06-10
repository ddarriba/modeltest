#ifndef PARTITION_H
#define PARTITION_H

#include "global_defs.h"
#include "loggable.h"
#include "model.h"
#include "msa.h"
#include "tree.h"

namespace modeltest
{

#define EXCEPTION_PARTITION_EMP_FREQS   201
#define EXCEPTION_PARTITION_EMP_RATES   202
#define EXCEPTION_PARTITION_EMP_PINV    203

class Partition : public Loggable
{
public:
    Partition(partition_id_t _id,
              Msa &_msa,
              Tree &_tree,
              partition_descriptor_t _descriptor,
              std::vector<mt_index_t> candidate_models,
              mt_mask_t model_params);
    virtual ~Partition();

    const partition_descriptor_t get_descriptor( void ) const;
    const std::string get_name( void ) const;

    void sort_models(bool forwards);

    mt_size_t get_number_of_models( void ) const;
    mt_size_t get_n_sites( void ) const;
    mt_size_t get_n_patterns( void ) const;
    std::vector<Model *> const& get_models( void ) const;
    Model * get_model(mt_index_t index) const;
    data_type_t get_datatype( void ) const;
    const partition_id_t get_id( void ) const;

    bool set_models(const std::vector<Model *> &models);

    const char * get_sequence(mt_index_t idx) const;
    const mt_size_t * get_weights( void ) const;

    std::vector<double> const& get_empirical_frequencies( void ) const;
    std::vector<double> const& get_empirical_subst_rates( void ) const;
    double get_empirical_pinv( void ) const;

    /* Logging functions */
    virtual void output_log(std::ostream  &out);
    virtual void input_log(std::istream  &in);
private:
    partition_id_t id;                   //! id of the partition
    Msa & msa;                           //! input MSA
    Tree & tree;                         //! user defined tree (optional)
    mt_size_t n_sites;                   //! number of sites
    mt_size_t n_patterns;                //! number of unique patterns
    partition_descriptor_t descriptor;   //! partition descriptor
    std::vector<double> emp_freqs;       //! empirical frequencies
    std::vector<double> emp_subst_rates; //! empirical substitution rates
    double emp_pinv;                     //! empirical prop. invar.
    
    std::vector<Model *> c_models;       //! candidate models

    /**
     * @brief Compute the empirical frequencies
     * @param[in] smooth if true, assign a minimum frequency
     *                   if false, fail with zero frequencies
     * @return true, if success
     */
    bool compute_empirical_frequencies(bool smooth);

    /**
     * @brief Compute the empirical substitution rates
     * @return true, if success
     */
    bool compute_empirical_subst_rates( void );

    /**
     * @brief Compute the empirical proportion of invariant sites
     * @return true, if success
     */
    bool compute_empirical_pinv( void );
};

} /* modeltest */
#endif // PARTITION_H
