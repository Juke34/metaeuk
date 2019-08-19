#include "Command.h"
#include "LocalCommandDeclarations.h"
#include "LocalParameters.h"

const int NO_CITATION = 0;
const char* binary_name = "metaeuk";
const char* tool_name = "metaeuk";
const char* tool_introduction = "Eukaryotic gene-discovery in metagenomic contigs";
const char* main_author = "Eli Levy Karin, eli.levy.karin@gmail.com";
const char* show_extended_help = "1";
const char* show_bash_info = "1";
bool hide_base_commands = true;

LocalParameters& localPar = LocalParameters::getLocalInstance();
std::vector<struct Command> commands = {
        // Main tools (workflows for non-experts)
        {"predictexons",             predictexons,            &localPar.predictexonsworkflow,    COMMAND_MAIN,
                "Predict eukaryotic exons based on protein similarity",
                "An analog of 6-frame translation to produce putative protein fragments. Search against protein DB. Compatible exon set identified with respect to each target. Two dynamic programming outputs: predictexonsBaseName_dp_protein_contig_strand_map and predictexonsBaseName_dp_optimal_exon_sets",
                "Eli Levy Karin <eli.levy.karin@gmail.com>",
                "<i:contigsDB> <i:targetsDB> <o:predictexonsBaseName> <tmpDir>",
                NO_CITATION, {{"contigsDB", DbType::ACCESS_MODE_INPUT, DbType::NEED_DATA, &DbValidator::nuclDb},
                                {"targetsDB", DbType::ACCESS_MODE_INPUT, DbType::NEED_DATA, &DbValidator::sequenceDb},
                                {"predictexonsBaseName", DbType::ACCESS_MODE_OUTPUT, DbType::NEED_DATA, NULL},
                                {"tmpDir", DbType::ACCESS_MODE_OUTPUT, DbType::NEED_DATA, &DbValidator::directory}}},
        {"reduceredundancy",             reduceredundancy,            &localPar.reduceredundancyworkflow,    COMMAND_MAIN,
                "A greedy approach to group MetaEuk predictions which share an exon",
                "A protein coding gene can be predicted more than once due to target DB homologies. A cluster representative is selected. Predictions in a cluster share an exon with the representative. Outputs: reduceRedundBaseName_grouped_predictions, reduceRedundBaseName_dp_protein_contig_strand_map, reduceRedundBaseName_dp_optimal_exon_sets, reduceRedundBaseName_grouped_predictions_no_overlap, reduceRedundBaseName_no_overlap_dp_protein_contig_strand_map and reduceRedundBaseName_no_overlap_dp_optimal_exon_sets",
                "Eli Levy Karin <eli.levy.karin@gmail.com>",
                "<i:dp_protein_contig_strand_map> <i:dp_optimal_exon_sets> <o:reduceRedundBaseName> <tmpDir>",
                NO_CITATION, {{"dp_protein_contig_strand_map", DbType::ACCESS_MODE_INPUT, DbType::NEED_DATA, &DbValidator::genericDb},
                                {"dp_optimal_exon_sets", DbType::ACCESS_MODE_INPUT, DbType::NEED_DATA, &DbValidator::genericDb},
                                {"reduceRedundBaseName", DbType::ACCESS_MODE_OUTPUT, DbType::NEED_DATA, NULL},
                                {"tmpDir", DbType::ACCESS_MODE_OUTPUT, DbType::NEED_DATA, &DbValidator::directory}}},
        {"unitesetstofasta",             unitesetstofasta,            &localPar.unitesetstofasta,    COMMAND_MAIN,
                "Create a fasta output from optimal exon sets",
                "Each optimal set is joined to a single sequence of codons or amino-acids",
                "Eli Levy Karin <eli.levy.karin@gmail.com>",
                "<i:contigsDB> <i:targetsDB> <i:predictedExonsDB> <o:unitedExonsFasta>",
                NO_CITATION, {{"contigsDB", DbType::ACCESS_MODE_INPUT, DbType::NEED_DATA, &DbValidator::nuclDb},
                                {"targetsDB", DbType::ACCESS_MODE_INPUT, DbType::NEED_DATA, &DbValidator::sequenceDb},
                                {"predictedExonsDB", DbType::ACCESS_MODE_INPUT, DbType::NEED_DATA, &DbValidator::genericDb},
                                {"unitedExonsFasta", DbType::ACCESS_MODE_OUTPUT, DbType::NEED_DATA, NULL}}},
        // taxonomy workflow is under construction...
        {"assigntaxonomy",             assigntaxonomy,            &localPar.assigntaxonomyworkflow,    COMMAND_HIDDEN,
                "Assign taxonomy to each representative prediction using 2bLCA",
                "Assign taxonomy to each representative prediction using 2bLCA against a uniprot-based refernce database. Contigs are assigned based on their predictions",
                "Eli Levy Karin <eli.levy.karin@gmail.com>",
                "<i:metaeukBaseName> <i:uniprotFasta> <tmpDir>",
                NO_CITATION, {{"",DbType::ACCESS_MODE_INPUT, DbType::NEED_DATA, NULL}}},
        // internal modules (COMMAND_EXPERT)
        {"resultspercontig",             resultspercontig,            &localPar.collectoptimalset,    COMMAND_EXPERT,
                "Swap fragments against targets search and join with contigs",
                "Each contig lists all target hits, mediated by the extracted putative fragments from that contig",
                "Eli Levy Karin <eli.levy.karin@gmail.com>",
                "<i:contigsDb> <i:fragmentsDb> <i:fragmentToTargetSearchRes> <o:contigToSearchRes>",
                NO_CITATION, {{"contigsDb", DbType::ACCESS_MODE_INPUT, DbType::NEED_DATA, &DbValidator::sequenceDb},
                                {"fragmentsDb", DbType::ACCESS_MODE_INPUT, DbType::NEED_DATA, &DbValidator::sequenceDb},
                                {"fragmentToTargetSearchRes", DbType::ACCESS_MODE_INPUT, DbType::NEED_DATA, &DbValidator::resultDb},
                                {"contigToSearchRes", DbType::ACCESS_MODE_OUTPUT, DbType::NEED_DATA, NULL}}},
        {"collectoptimalset",             collectoptimalset,            &localPar.collectoptimalset,    COMMAND_EXPERT,
                "Collect the optimal set of exons for a target protein/profile",
                "A dynamic programming procedure on all candidates of each contig and strand combination",
                "Eli Levy Karin <eli.levy.karin@gmail.com>",
                "<i:contigToSearchRes> <i:targetsDB> <o:predictedExonsDB>",
                NO_CITATION,{{"contigToSearchRes", DbType::ACCESS_MODE_INPUT, DbType::NEED_DATA, &DbValidator::resultDb},
                                {"targetsDB", DbType::ACCESS_MODE_INPUT, DbType::NEED_DATA, &DbValidator::sequenceDb},
                                {"predictedExonsDB", DbType::ACCESS_MODE_OUTPUT, DbType::NEED_DATA, NULL}}},
        {"grouppredictions",             grouppredictions,            &localPar.onlythreads,    COMMAND_EXPERT,
                "Assignment of predictions to cluster",
                "A greedy examination of predictions accoridng to their contig order, subordered by the number of exons. Predictions in a cluster share an exon with the representative.",
                "Eli Levy Karin <eli.levy.karin@gmail.com>",
                "<i:contigStrandSortedMap> <o:groupedPredictionsDB> <o:groupedPredictionsDBNoOverlap>",
                NO_CITATION, {{"contigStrandSortedMap", DbType::ACCESS_MODE_INPUT, DbType::NEED_DATA, &DbValidator::genericDb},
                                {"groupedPredictionsDB", DbType::ACCESS_MODE_OUTPUT, DbType::NEED_DATA, &DbValidator::clusterDb},
                                {"groupedPredictionsDBNoOverlap", DbType::ACCESS_MODE_OUTPUT, DbType::NEED_DATA, &DbValidator::clusterDb}}},
        // overwrite MMseqs2 swapdb because of its input type validation
        {"swapdb",               swapdb,               &localPar.swapdb,               COMMAND_HIDDEN,
                "Create a DB where the key is from the first column of the input result DB",
                NULL,
                "Martin Steinegger <martin.steinegger@mpibpc.mpg.de>, Clovis Galiez & Eli Levy Karin",
                "<i:resultDB> <o:resultDB>",
                NO_CITATION, {{"resultDB",  DbType::ACCESS_MODE_INPUT, DbType::NEED_DATA, &DbValidator::allDb },
                                {"resultDB", DbType::ACCESS_MODE_OUTPUT, DbType::NEED_DATA, &DbValidator::allDb}}}
};
