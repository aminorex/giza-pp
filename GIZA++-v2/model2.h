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

#ifndef GIZAPP_MODEL2_H_
#define GIZAPP_MODEL2_H_

#include "model1.h"

#include "ttables.h"
#include "atables.h"

class Perplexity;
class SentenceHandler;

class Model2 : public Model1 {
 public:
  // TODO: Should be private.
  AModel<PROB>& aTable;
  AModel<COUNT>& aCountTable;

  Model2(Model1& m1, AModel<PROB>& _aTable, AModel<COUNT>& _aCountTable);
  virtual ~Model2();

  void initialize_table_uniformly(SentenceHandler&);

  int em_with_tricks(int);

  void load_table(const char* aname);

  AModel<PROB>& getATable() { return aTable; }
  AModel<COUNT>& getACountTable() { return aCountTable; }

  void em_loop(Perplexity& perp,
               SentenceHandler& sHandler1,
               bool dump_files,
               const char* alignfile,
               Perplexity&, bool test);

 private:
  friend class Model3;
};

#endif  // GIZAPP_MODEL2_H_
