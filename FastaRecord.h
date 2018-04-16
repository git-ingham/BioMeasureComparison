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
    FastaRecord(std::string iid, std::string iseq) {
        id = iid;
        seq = iseq;
    };
    FastaRecord() {};
    std::string get_id() const {
        return id;
    };
    std::string get_seq() const {
        return seq;
    };
    bool operator<(const FastaRecord& rhs) const {
        return seq.compare(rhs.get_seq()) < 0;
    };
    void readsinglefasta(std::ifstream&);
    void test(void) {}; //!< @todo implement this
};

typedef std::vector<FastaRecord> fastavec_t;
fastavec_t readfastafile(const std::string&);

#endif // FASTA_H
