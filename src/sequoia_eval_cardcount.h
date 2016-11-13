
#ifndef SEQUOIA_EVAL_CARD_COUNTING_H
#define SEQUOIA_EVAL_CARD_COUNTING_H

#include "sequoia_eval.h"
#include <iostream>
#include <vector>
using namespace std;
namespace sequoia {

/**
 * The CardCountingEvaluation counts for each k = 0 ... n the number of solutions of size k
 * for the first free set variable.
 * See page 48 in http://darwin.bth.rwth-aachen.de/opus3/volltexte/2013/4531/pdf/4531.pdf
 */
class  CardCountingEvaluation : public SequoiaInternalEvaluation {
    typedef std::vector<unsigned long> value_t;
public:
    ~CardCountingEvaluation() {
        CONCUR_VECTOR<const value_t*>::const_iterator it;
        for (it = _values.begin(); it != _values.end(); it++) {
            delete *it;
        }
    }

    const void * elem(const sequoia::Bag *bag,
                      const sequoia::Assignment *alpha) {
        assert(_free_symbols.size() > 0);
        
        const sequoia::SetMove *smove = alpha->get(_free_symbols[0]);
        int size = 0;

        // count elements in this set
        for (unsigned int i = 0; i < _terminal_symbols.size(); ++i) {
            const sequoia::ConstantSymbol* t = _terminal_symbols[i];
            unsigned int t_num = _terminal_symbols[i]->nesting_depth()
                - _terminal_symbols[0]->nesting_depth();
            if (smove->test(t)) {
                size++;
            }
        }
        value_t *res = new value_t(size+1);
        (*res)[size] = 1; // 1 solution of this size
        _values.push_back(res);
        return res;
    }

    const void * mult(const void *e1, const void *e2, const void *intersect) {
        const value_t *i1 = (const value_t *) e1;
        const value_t *i2 = (const value_t *) e2;
        const value_t *is = (const value_t *) intersect;

        int sz_i1 = i1->size();
        int sz_i2 = i2->size();
        int sz_is = i2->size();

        // get size of intersection
        long is_size = 0;
        for (int i = 0; i < is->size(); i++) {
            long num = (*is)[i];
            if (num > 0) {
                is_size = i;
                break;
            }
        }

        value_t *res = new value_t(sz_i1 + sz_i2 - is_size - 1); // -1 because size 0 is counted twice
        _values.push_back(res);

        for (int i = is_size; i < sz_i1; i++) {                
            long v1 = (*i1)[i];
            for (int j = is_size; j < sz_i2; j++) {            
                long v2 = (*i2)[j];
                // elements in the intersection are counted twice
                (*res)[i + j - is_size] += v1 * v2;
            }
        }
        return res;
    }

    const void * add(const void *e1, const void *e2) {
        value_t *i1 = (value_t *) e1;
        value_t *i2 = (value_t *) e2;
        value_t *res = new value_t();
        _values.push_back(res);

        int sz_i1 = i1->size();
        int sz_i2 = i2->size();
        int max_size = max(sz_i1, sz_i2);
        
        for (int i = 0; i < max_size; i++) {            
            long v1 = i < sz_i1 ? (*i1)[i] : 0;
            long v2 = i < sz_i2 ? (*i2)[i] : 0;
            res->push_back(v1 + v2);
        }
        return res;
    }

    void output_solution(std::ostream &outs, const void *e) const {
        value_t *res = (value_t *) e;
        for (unsigned int i = 0; i < res->size(); i++)
            outs << i << ":\t" << (*res)[i] << std::endl;
    }

    bool returns_vertex_sets() const {
        return false;
    }

    vertex_sets_t
    convert_to_vertex_sets(const void *e) const {
        return vertex_sets_t();
    }

private:
    CONCUR_VECTOR<const value_t*> _values;
};

} // namespace

#endif // SEQUOIA_EVAL_CARD_COUNTING_H
