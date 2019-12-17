#include <seqan3/io/sequence_file/output.hpp>

//! [sequence_input_include]
#include <seqan3/core/debug_stream.hpp>      // for debug_stream
#include <seqan3/io/sequence_file/input.hpp> // for sequence_file_input
//! [sequence_input_include]

//! [alignment_include]
#include <tuple>                                                          // for std::make_pair
#include <seqan3/alignment/aligned_sequence/aligned_sequence_concept.hpp> // for alignment stream operator
#include <seqan3/alignment/pairwise/align_pairwise.hpp>                   // for align_pairwise
//! [alignment_include]

using seqan3::operator""_dna4;

int main()
{
    auto tmp_dir = std::filesystem::temp_directory_path();
    std::string filename{tmp_dir/"seq.fasta"};
    {
        // Create a /tmp/my.fasta file.
        seqan3::sequence_file_output file_out{filename};
        file_out.emplace_back("ACGTGATG"_dna4, std::string{"seq1"});
        file_out.emplace_back("AGTGATACT"_dna4, std::string{"seq2"});
    }

//! [sequence_input]
    // Initialise a file input object with a FastA file.
    seqan3::sequence_file_input file_in{filename}; // filename: "seq.fasta"

    // Retrieve the sequences and ids.
    for (auto &[seq, id, qual] : file_in)
    {
        seqan3::debug_stream << "ID:  " << id << '\n';
        seqan3::debug_stream << "SEQ: " << seq << '\n';
        seqan3::debug_stream << "EMPTY QUAL." << qual << '\n'; // qual is empty for FastA files
    }
//! [sequence_input]

    using seqan3::operator""_dna5;
    std::vector<seqan3::dna5_vector> sequences{"ACGTGATG"_dna5, "AGTGATACT"_dna5};
//! [alignment]
    // Call a pairwise alignment with edit distance and traceback.
    for (auto && res : align_pairwise(std::tie(sequences[0], sequences[1]),
                                      seqan3::align_cfg::edit | seqan3::align_cfg::result{seqan3::with_alignment}))
    {
        // Print the resulting score and the alignment.
        seqan3::debug_stream << res.score() << '\n';      // => -4
        seqan3::debug_stream << res.alignment() << '\n';  // =>       0     .    :
                                                          //            ACGTGATG--
                                                          //            | |||||
                                                          //            A-GTGATACT
    }
//! [alignment]
    std::filesystem::remove(filename);
    return 0;
}
