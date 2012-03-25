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

/* perplexity.h
 * ============
 * Mike Jahr, 7/15/99
 * Machine Translation group, WS99
 * Center for Language and Speech Processing
 *
 * Last Modified by: Yaser Al-Onaizan, August 17, 1999
 *
 * Simple class used to calculate cross entropy and perplexity
 * of models.
 */

#ifndef GIZAPP_UTIL_PERPLEXITY_H_
#define GIZAPP_UTIL_PERPLEXITY_H_

#include <algorithm>
#include <cmath>
#include <ostream>

#include "util/vector.h"
#include "util/array2.h"

const double kCrossEntropyBase = 2.0;

class Perplexity {
 private:
  double sum_;
  double wc_;
  Array2<double, Vector<double> > *E_M_L_;
  Vector<string> model_id_;
  Vector<double> perp_;
  Vector<double> ce_;
  Vector<string> name_;

  void Init();

 public:
  Perplexity();
  ~Perplexity();

  void clear() {
    sum_ = 0.0;
    wc_ = 0.0;
  }

  size_t size() const { return(std::min(perp_.size(), ce_.size())); }

  double perplexity() const { return std::exp(-1 * sum_ / wc_); }

  double cross_entropy() const {
    return (-1.0 * sum_ / (std::log(kCrossEntropyBase) * wc_));
  }

  double getSum() const { return sum_ ; }
  double word_count() const { return wc_; }

  void addFactor(const double p, const double count, const int l,
                 const int m, bool withPoisson) {
    wc_ += count * m ; // number of french words
    sum_ += count * ((withPoisson ? ((*E_M_L_)(l, m)) : 0.0) + p) ;
  }

  void record(const std::string& model) {
    model_id_.push_back(model);
    perp_.push_back(perplexity());
    ce_.push_back(cross_entropy());
  }

  friend void generatePerplexityReport(const Perplexity&, const Perplexity&,
                                       const Perplexity&, const Perplexity&,
                                       std::ostream&, int, int, bool);
};

#endif  // GIZAPP_UTIL_PERPLEXITY_H_