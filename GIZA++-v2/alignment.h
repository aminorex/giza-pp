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

/*
  alignment: 'checked' alignment representation with autom.
  calc. of fertilities

  Franz Josef Och (30/07/99)
*/
#ifndef GIZAPP_ALIGNMENT_H_
#define GIZAPP_ALIGNMENT_H_

#include "util/vector.h"
#include <cassert>
#include "defs.h"
#include "util/assert.h"

class Alignment {
 private:
  struct AlignmentStruct {
    AlignmentStruct() : prev(0), next(0) { }
    PositionIndex prev, next;
  };

  Vector<PositionIndex> a;
  Vector<PositionIndex> positionSum,f;

 public:
  Vector<PositionIndex> als_i;
  Vector<AlignmentStruct>  als_j;
  PositionIndex l,m;
  Alignment() {}
  Alignment(PositionIndex _l, PositionIndex _m)
    : a(_m+1, (PositionIndex)0),
    positionSum(_l+1, (PositionIndex)0), f(_l+1, (PositionIndex)0), als_i(_l+1,0),als_j(_m+1),l(_l), m(_m)
    {
      f[0]=m;
      for(PositionIndex j=1;j<=m;j++)
	{
	  if( j>1 )
	    als_j[j].prev= j-1;
	  if( j<m )
	    als_j[j].next= j+1;
	}
      als_i[0]=1;
    }

  virtual ~Alignment() {}

  PositionIndex get_l()const
    {return l;}
  PositionIndex get_m()const
    {return m;}
  void doMove(int i,int j)
    {
      set(j,i);
    }
  void doSwap(int j1,int j2)
    {
      int aj1=a[j1],aj2=a[j2];
      set(j1,aj2);
      set(j2,aj1);
    }
  void set(PositionIndex j, PositionIndex aj)
    {
      PositionIndex old_aj=a[j];
      MASSERT(j<a.size());MASSERT(aj<f.size());
      MASSERT(old_aj<f.size());MASSERT(f[old_aj]>0);
      MASSERT(j>0);
      positionSum[old_aj]-=j;
      // ausfuegen
      PositionIndex prev=als_j[j].prev;
      PositionIndex next=als_j[j].next;
      if( next )
	als_j[next].prev=prev;
      if( prev )
	als_j[prev].next=next;
      else
	als_i[old_aj]=next;

      // neue Position suchen
      PositionIndex lfd=als_i[aj],llfd=0;
      while( lfd && lfd<j )
	lfd = als_j[llfd=lfd].next;

      // einfuegen
      als_j[j].prev=llfd;
      als_j[j].next=lfd;
      if( llfd )
	als_j[llfd].next=j;
      else
	als_i[aj]=j;
      if( lfd )
	als_j[lfd].prev=j;

      f[old_aj]--;
      positionSum[aj]+=j;
      f[aj]++;
      a[j]=aj;
    }
  const Vector<PositionIndex>& getAlignment() const
    {return a ;}
  PositionIndex get_al(PositionIndex j)const
    {
      MASSERT(j<a.size());
      return a[j];
    }
  PositionIndex operator()(PositionIndex j)const
    {
      MASSERT(j<a.size());
      return a[j];
    }
  PositionIndex fert(PositionIndex i)const
    {
      MASSERT(i<f.size());
      return f[i];
    }
  PositionIndex get_head(PositionIndex i)const
    {
      MASSERT( als_i[i]==_get_head(i) );
      return als_i[i];
    }
  PositionIndex get_center(PositionIndex i)const
    {
      if( i==0 )return 0;
      MASSERT(((positionSum[i]+f[i]-1)/f[i]==_get_center(i)));
      return (positionSum[i]+f[i]-1)/f[i];
    }
  PositionIndex _get_head(PositionIndex i)const
    {
      if( fert(i)==0 )return 0;
      for(PositionIndex j=1;j<=m;j++)
	if( a[j]==i )
	  return j;
      return 0;
    }
  PositionIndex _get_center(PositionIndex i)const
    {
      if( i==0 )return 0;
      MASSERT(fert(i));
      PositionIndex sum=0;
      for(PositionIndex j=1;j<=m;j++)
	if( a[j]==i )
	  sum+=j;
      return (sum+fert(i)-1)/fert(i);
    }
  PositionIndex prev_cept(PositionIndex i)const
    {
      if( i==0 )return 0;
      PositionIndex k=i-1;
      while(k&&fert(k)==0)
	k--;
      return k;
    }
  PositionIndex next_cept(PositionIndex i)const
    {
      PositionIndex k=i+1;
      while(k<l+1&&fert(k)==0)
	k++;
      return k;
    }
  PositionIndex prev_in_cept(PositionIndex j)const
    {
      //PositionIndex k=j-1;
      //while(k&&a[k]!=a[j])
      //k--;
      //assert( als_j[j].prev==k );
      //assert(k);
      //return k;
      MASSERT(als_j[j].prev==0||a[als_j[j].prev]==a[j]);
      return als_j[j].prev;
    }
  friend ostream &operator<<(ostream&out, const Alignment&a);
  friend bool operator==(const Alignment&a, const Alignment&b)
    {
      MASSERT(a.a.size()==b.a.size());
      for(PositionIndex j=1;j<=a.get_m();j++)
	if(a(j)!=b(j))
	  return 0;
      return 1;
    }
  friend bool operator<(const Alignment&x, const Alignment&y)
    {
      MASSERT(x.get_m()==y.get_m());
      for(PositionIndex j=1;j<=x.get_m();j++)
	if( x(j)<y(j) )
	  return 1;
	else if( y(j)<x(j) )
	  return 0;
      return 0;
    }
  friend int differences(const Alignment&x, const Alignment&y){
    int count=0;
    MASSERT(x.get_m()==y.get_m());
    for(PositionIndex j=1;j<=x.get_m();j++)
      count += (x(j)!=y(j));
    return count;
  }
  bool valid()const
    {
      if( 2*f[0]>m )
	return 0;
      for(unsigned int i=1;i<=l;i++)
	if( f[i]>=g_max_fertility )
	  return 0;
      return 1;
    }
  friend class transpair_model5;
};

#endif  // GIZAPP_ALIGNMENT_H_
