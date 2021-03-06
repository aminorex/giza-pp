/*
  EGYPT Toolkit for Statistical Machine Translation

  Written by Yaser Al-Onaizan, Jan Curin, Michael Jahr, Kevin Knight, John Lafferty, Dan Melamed, David Purdy, Franz Och, Noah Smith, and David Yarowsky.

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
  USA.
*/

#ifndef GIZAPP_IBM_MODEL3_H_
#define GIZAPP_IBM_MODEL3_H_

#include "ibm_model2.h"
#include "ntables.h"
#include "defs.h"

class Alignment;
class AlignmentModel;
class HMM;
class Perplexity;
class SentenceHandler;
class transpair_model2;
class transpair_model3;
class TransPairModelHMM;

class IBMModel3 : public IBMModel2 {
  // TODO: should be private.
 public:
  AModel<PROB>  dTable;
  AModel<COUNT> dCountTable;

  PROB p0,p1;
  double p0_count, p1_count;

  nmodel<PROB>  nTable;
  nmodel<COUNT> nCountTable;
  HMM* h;

 public:
  IBMModel3(IBMModel2& m2);
  ~IBMModel3();

  void setHMM(HMM*_h) {h = _h; }

  // methods
  void transfer(SentenceHandler&, bool,
                Perplexity&,
                Perplexity&,
                bool updateT = true);

  // Performs simple Model 2 -> Model 3 transfer.
  // It sets values for n and p without considering Model 2's ideas.
  // It sets d values based on a.
  void transferSimple(SentenceHandler&, bool,
                      Perplexity&, Perplexity&,
                      bool updateT = true);

  void load_tables(const char *nfile,
                   const char *dfile,
                   const char *p0file);

  void em(int, SentenceHandler&);
  int viterbi(int, int, int,int);

 private:
  LogProb prob_of_special(Vector<WordIndex>&,
                          Vector<WordIndex>&,
                          TModel<COUNT, PROB>&,
                          Vector<WordIndex>&,
                          Vector<WordIndex>&);

  LogProb prob_of_target_and_alignment_given_source(Vector<WordIndex>&,
                                                    Vector<WordIndex>&,
                                                    TModel<COUNT, PROB>&,
                                                    Vector<WordIndex>&,
                                                    Vector<WordIndex>&);
  LogProb prob_of_target_given_source(TModel<COUNT, PROB>&,
                                      Vector<WordIndex>&,
                                      Vector<WordIndex>&);

  LogProb scoreOfMove(Vector<WordIndex>&, Vector<WordIndex>&,
                      Vector<WordIndex>&, Vector<WordIndex>&,
                      TModel<COUNT, PROB>&, WordIndex, WordIndex);

  LogProb scoreOfSwap(Vector<WordIndex>&, Vector<WordIndex>&,
                      Vector<WordIndex>&, TModel<COUNT, PROB>&, int, int);

  void hillClimb(Vector<WordIndex>&, Vector<WordIndex>&,
                 Vector<WordIndex>&, Vector<WordIndex>&,
                 LogProb&, TModel<COUNT, PROB>&, int, int);

  void findBestAlignment(Vector<WordIndex>&, Vector<WordIndex>&,
                         Vector<WordIndex>&, Vector<WordIndex>&,
                         LogProb&,int , int);

  void findAlignmentsNeighborhood(Vector<WordIndex>&,
                                  Vector<WordIndex>&,
                                  LogProb&align_total_count,
                                  AlignmentModel& neighborhood,
                                  int, int);

  void collectCountsOverAlignement(const Vector<WordIndex>& es,
                                   const Vector<WordIndex>& fs,
                                   const Vector<WordIndex>&,
                                   LogProb , float count);

  LogProb viterbi_model2(const transpair_model3&ef,
                         Alignment&output,
                         int pair_no,
                         int i_peg = -1,
                         int j_peg = -1) const;

  LogProb _viterbi_model2(const transpair_model2&ef,
                          Alignment&output,
                          int i_peg = -1,
                          int j_peg = -1) const;

  LogProb viterbi_model2(const TransPairModelHMM&ef,
                         Alignment& output,
                         int pair_no,
                         int i_peg = -1,
                         int j_peg = -1) const;

  void estimate_t_a_d(SentenceHandler& sHandler1,
                      Perplexity& perp1,
                      Perplexity& perp2,
                      bool simple,
                      bool dump_files,
                      bool updateT);

  void viterbi_loop(Perplexity&,
                    Perplexity&,
                    SentenceHandler&,
                    bool,
                    const char*,
                    bool,
                    string model);

  template<class MODEL_TYPE, class A,class B>
  void viterbi_loop_with_tricks(Perplexity&,
                                Perplexity&,
                                SentenceHandler&,
                                bool,
                                const char*,
                                bool,
                                string model,
                                bool final,
                                A* d4m,
                                B* d5m);
};

#endif  // GIZAPP_IBM_MODEL3_H_
