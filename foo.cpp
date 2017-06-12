#include <iostream>
#include <string>
#include <map>

                 // ngram     count
typedef std::map<std::string, unsigned int> ngram_t;
               // sequence    ngrams
typedef std::map<std::string, ngram_t> ngrams_t;

int main(int argc, char *argv[]) {
    ngram_t a = 
        {
	    { "defg", 1 },
	    { "hijk", 2 },
	    { "lmno", 3 }
        };
    ngram_t b = 
        {
	    { "stuv", 4 },
	    { "wxyz", 5 }
        };
    ngrams_t n = { {"abc", a}, {"pqr", b}};

    const std::string aseq = "abc";
    if (n.count(aseq) > 0)
	ngram_t agrams = n[aseq];
}
