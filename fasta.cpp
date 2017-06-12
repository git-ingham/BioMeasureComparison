#include "fasta.h"

#include <cstring>
#include <errno.h>
#include <err.h>

// Read one record from a FASTA file.
// Assumption: we are positioned ready to read the first character of
// the first/next sequence entry

FastaRecord
readsinglefasta(std::ifstream& is) 
{
    static unsigned int line = 1;
    std::string id, seq;
    const std::string bases = "ABCDEFGHIKLMNPQRSTUVWXYZ-*abcdefghiklmnpqrstuvwxyz";

    if (is.eof()) {
        std::cerr << "end of file before >id";
    }

    if (is.peek() != '>') {
        std::cerr << "Next character is not a '>' as it should be on line ";
	std::cerr << line << std::endl;
    }

    is.ignore(1); // The >
    std::getline(is, id);
    line++;

    if (is.eof()) {
        std::cerr << "end of file before sequence";
    }

    // FASTA sequence can be multiple lines
    seq = "";
    while (!is.eof() && is.peek() != '>') {
	std::string s;
        std::getline(is, s);
	seq += s;
	line++;
    }
    std::string::size_type p;
    if ((p = seq.find_first_not_of(bases)) != std::string::npos) {
        std::cerr << "Invalid character at position " << p << " in '" << seq;
	std::cerr << std::endl;
	exit(1);
    }

    return FastaRecord(id, seq);
}

fastavec_t readfastafile(const std::string& fastafile)
{
    std::ifstream f;
    f.open(fastafile);
    if (!f.is_open()) {
        std::cerr << "cannot open '" << fastafile << "' for reading: ";
	std::cerr << strerror(errno) << std::endl;
	exit(1);
    }

    fastavec_t sequences;
    for (unsigned int i=0; !f.eof(); ++i) {
	FastaRecord seq = readsinglefasta(f);
	sequences.emplace_back(seq);
    }

    return sequences;
}
