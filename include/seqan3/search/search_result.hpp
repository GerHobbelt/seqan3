// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief Provides seqan3::search_result.
 * \author Enrico Seiler <enrico.seiler AT fu-berlin.de>
 * \author Svenja Mehringer <svenja.mehringer AT fu-berlin.de>
 */

#pragma once

#include <seqan3/std/concepts>
#include <exception>

#include <seqan3/core/configuration/configuration.hpp>
#include <seqan3/core/debug_stream/debug_stream_type.hpp>
#include <seqan3/core/detail/empty_type.hpp>
#include <seqan3/core/detail/template_inspection.hpp>
#include <seqan3/search/fm_index/concept.hpp>
#include <seqan3/search/fm_index/bi_fm_index_cursor.hpp>
#include <seqan3/search/fm_index/fm_index_cursor.hpp>

namespace seqan3::detail
{
// forward declaration
template <typename search_configuration_t>
#if !SEQAN3_WORKAROUND_GCC_93467
    requires is_type_specialisation_of_v<search_configuration_t, configuration>
#endif // !SEQAN3_WORKAROUND_GCC_93467
struct policy_search_result_builder;
} // namespace seqan3::detail

namespace seqan3
{

/*!\brief The result class generated by the seqan3::seach algorithm.
 * \ingroup search
 * \tparam query_id_type The type of the query_id; must model std::integral.
 * \tparam cursor_type The type of the cursor; must model seqan3::detail::template_specialisation_of a
 * seqan3::fm_index_cursor or a seqan3::bi_fm_index_cursor
 * \tparam reference_id_type The type of the reference_id; must model std::integral.
 * \tparam reference_begin_position_type The type of the reference_begin_position; must model std::integral.
 *
 * \if DEV
 * \note If the information is not available, the respective data type is seqan3::detail::empty_type.
 * \endif
 *
 * The seqan3::search algorithm returns a range of hits. A single hit is stored in a seqan3::search_result.
 * By default, the search result contains the query id, the reference id where the query matched and the begin
 * position in the reference where the query sequence starts to match the reference sequence.
 * Those information can be accessed via the respective member functions.
 *
 * The following member functions exist:
 *
 * * seqan3::search_result::query_id()
 * * seqan3::search_result::index_cursor()
 * * seqan3::search_result::reference_id()
 * * seqan3::search_result::reference_begin_position()
 *
 * Note that the index cursor is not included in a hit by default. If you are trying to use the respective member
 * function, a static_assert will prevent you from doing so. You can configure the result of the search with the
 * \ref search_configuration_subsection_output "output configuration".
 */
template <typename query_id_type,
          typename cursor_type,
          typename reference_id_type,
          typename reference_begin_position_type>
//!\cond
    requires (std::integral<query_id_type> || std::same_as<query_id_type, detail::empty_type>) &&
             (detail::template_specialisation_of<cursor_type, fm_index_cursor> ||
                     detail::template_specialisation_of<cursor_type, bi_fm_index_cursor> ||
                     std::same_as<cursor_type, detail::empty_type>) &&
             (std::integral<reference_id_type> || std::same_as<reference_id_type, detail::empty_type>) &&
             (std::integral<reference_begin_position_type> || std::same_as<reference_begin_position_type,
                                                                           detail::empty_type>)
//!\endcond
class search_result
{
private:
    //!\brief Stores the query_id of the search result.
    query_id_type query_id_{};
    //!\brief Stores the cursor of the search result.
    cursor_type cursor_{};
    //!\brief Stores the reference_id of the search result.
    reference_id_type reference_id_{};
    //!\brief Stores the reference_begin_position of the search result.
    reference_begin_position_type reference_begin_position_{};

    // Grant the policy access to private constructors.
    template <typename search_configuration_t>
    #if !SEQAN3_WORKAROUND_GCC_93467
    //!\cond
        requires detail::is_type_specialisation_of_v<search_configuration_t, configuration>
    //!\endcond
    #endif // !SEQAN3_WORKAROUND_GCC_93467
    friend struct detail::policy_search_result_builder;

public:
    /*!\name Constructors, destructor and assignment
     * \{
     */
    search_result() = default; //!< Defaulted.
    search_result(search_result const &) = default; //!< Defaulted.
    search_result(search_result &&) = default; //!< Defaulted.
    search_result & operator=(search_result const &) = default; //!< Defaulted.
    search_result & operator=(search_result &&) = default; //!< Defaulted.
    ~search_result() = default; //!< Defaulted.

    //!\}

    /*!\name Accessors
     * \brief Functions to access elements of the search result.
     * \{
     */
    //!\brief Returns the id of the query which produced this search result.
    constexpr auto query_id() const noexcept
    {
        static_assert(!std::same_as<query_id_type, detail::empty_type>,
                      "You tried to access the query_id but it was not selected in the output "
                      "configuration of the search.");

        return query_id_;
    }

    /*!\brief Returns the index cursor pointing to the suffix array range where the query was found.
     * \sa seqan3::fm_index_cursor
     * \sa seqan3::bi_fm_index_cursor
     */
    constexpr auto index_cursor() const noexcept(!(std::same_as<cursor_type, detail::empty_type>))
    {
        static_assert(!std::same_as<cursor_type, detail::empty_type>,
                      "You tried to access the index cursor but it was not selected in the output "
                      "configuration of the search.");

        return cursor_;
    }

    /*!\brief Returns the reference id where the query was found.
     *
     * \details
     * The reference id is an arithmetic value that corresponds to the index of the reference text in the index.
     * The order is determined on construction of the index.
     */
    constexpr auto reference_id() const noexcept(!(std::same_as<reference_id_type, detail::empty_type>))
    {
        static_assert(!std::same_as<reference_id_type, detail::empty_type>,
                      "You tried to access the reference id but it was not selected in the output "
                      "configuration of the search.");

        return reference_id_;
    }

    //!\brief Returns the reference begin positions where the query was found in the reference text (at `reference id`).
    constexpr auto reference_begin_position() const
        noexcept(!(std::same_as<reference_begin_position_type, detail::empty_type>))
    {
        static_assert(!std::same_as<reference_begin_position_type, detail::empty_type>,
                      "You tried to access the reference begin position but it was not selected in the "
                      "output configuration of the search.");

        return reference_begin_position_;
    }
    //!\}

    /*!\name Comparison
     * \{
     */
    //!\brief Returns whether `lhs` and `rhs` are the same.
    friend bool operator==(search_result const & lhs, search_result const & rhs) noexcept
    {
        bool equality = lhs.query_id_ == rhs.query_id_;
        if constexpr (!std::is_same_v<cursor_type, detail::empty_type>)
            equality &= lhs.cursor_ == rhs.cursor_;
        if constexpr (!std::is_same_v<reference_id_type, detail::empty_type>)
            equality &= lhs.reference_id_ == rhs.reference_id_;
        if constexpr (!std::is_same_v<reference_begin_position_type, detail::empty_type>)
            equality &= lhs.reference_begin_position_ == rhs.reference_begin_position_;

        return equality;
    }

    //!\brief Returns whether `lhs` and `rhs` are not the same.
    friend bool operator!=(search_result const & lhs, search_result const & rhs) noexcept
    {
        return !(lhs == rhs);
    }
    //!\}
};

/*!\brief Print the seqan3::search_result to seqan3::debug_stream.
 * \tparam char_t The underlying character type of the seqan3::debug_stream_type.
 * \tparam search_result_t A specialization of seqan3::search_result.
 * \param stream The stream.
 * \param result The search result to print.
 * \relates seqan3::debug_stream_type
 */
template <typename char_t, typename search_result_t>
//!\cond
    requires detail::is_type_specialisation_of_v<std::remove_cvref_t<search_result_t>, search_result>
//!\endcond
inline debug_stream_type<char_t> & operator<<(debug_stream_type<char_t> & stream, search_result_t && result)
{
    using result_type_list = detail::transfer_template_args_onto_t<std::remove_cvref_t<search_result_t>, type_list>;

    stream << "<";
    if constexpr (!std::same_as<list_traits::at<0, result_type_list>, detail::empty_type>)
        stream << "query_id:" << result.query_id();
    if constexpr (!std::same_as<list_traits::at<1, result_type_list>, detail::empty_type>)
        stream << ", index cursor is present";
    if constexpr (!std::same_as<list_traits::at<2, result_type_list>, detail::empty_type>)
        stream << ", reference_id:" << result.reference_id();
    if constexpr (!std::same_as<list_traits::at<3, result_type_list>, detail::empty_type>)
        stream << ", reference_pos:" << result.reference_begin_position();
    stream << ">";

    return stream;
}

} // namespace seqan3
