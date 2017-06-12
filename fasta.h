// Sigh, I should not have to reinvent the wheel here.

#ifndef FASTA_H
#define FASTA_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class FastaRecord {
	std::string id, seq;
    public:
	FastaRecord(std::string iid, std::string iseq) {id = iid; seq = iseq;};
	std::string get_id() const {return id;};
	std::string get_seq() const {return seq;};
};

typedef std::vector<FastaRecord> fastavec_t;
FastaRecord readsinglefasta(std::ifstream&);
fastavec_t readfastafile(const std::string&);

#endif // FASTA_H
