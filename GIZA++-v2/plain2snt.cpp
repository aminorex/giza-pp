#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <cstdlib>
#include <iterator>

using namespace std;

int main(int argc,char**argv)
{
    vector<double>weights;
    vector<string>filenames;
    vector<string>vcbpaths;
    
    for (int i=1;i<argc;++i)
        if (string(argv[i])=="-weight")
            weights.push_back(atof(argv[++i]));
        else if (string(argv[i])=="-vcb")
            vcbpaths.push_back(argv[++i]);
        else
            filenames.push_back(argv[i]);
  
    if ((filenames.size()%2)==1||filenames.size()==0 || 
        (!vcbpaths.empty()&&(filenames.size()!=2||weights.size()!=0||vcbpaths.size()!=2)))
    {
        cerr << argv[0] << " txt1 txt2 ([-vcb vcb1 -vcb vcb2]|[txt3 txt4 -weight w])\n";
        cerr << " Converts plain text into GIZA++ snt-format.\n";
        exit(1);
    }
    string line1,line2,word;
    map<string,int> v1,v2;
    map<string,int> vi1,vi2;
    map<string,int> id1,id2;
    vector<string> iid1(2),iid2(2);
  
    string w1(filenames[0]);
    string w2(filenames[1]);
  
    if (w1.length()>4 && w2.length()>4 &&
        ((w1.substr(w1.length()-4,w1.length())==".tok" && 
          w2.substr(w2.length()-4,w2.length())==".tok" )||
         (w1.substr(w1.length()-4,w1.length())==".txt" && 
          w2.substr(w2.length()-4,w2.length())==".txt" )))
    {
        w1=w1.substr(0,w1.length()-4);
        w2=w2.substr(0,w2.length()-4);
        cerr << "w1:"<< w1 << " w2:" << w2 << endl;
    } 
      

    string vocab1(w1),vocab2(w2),snt1,snt2;
    unsigned int slashpos=static_cast<unsigned int>(vocab1.rfind('/'))+1;
    if ( slashpos>=vocab1.length() ) slashpos=0;
    string vocab1x(vocab1.substr(slashpos,vocab1.length()));
    cout << vocab1 << " -> " << vocab1x << endl;
    slashpos=static_cast<unsigned int>(vocab2.rfind('/'))+1;
    if ( slashpos>=vocab2.length() ) slashpos=0;
    string vocab2x(vocab2.substr(slashpos,vocab2.length()));
    cout << vocab2 << " -> " << vocab2x << endl;  
    snt1=vocab1+"_"+vocab2x+string(".snt");
    snt2=vocab2+"_"+vocab1x+string(".snt");
    vocab1+=string(".vcb");
    vocab2+=string(".vcb");
  
    // if an input vocabulary pair is indicated, read them
    bool ivocab_p = false;
    if (!vcbpaths.empty()) 
    {
        ifstream ivocab1(vcbpaths[0].c_str()),ivocab2(vcbpaths[1].c_str());
        string vcbent;
        while (getline(ivocab1,vcbent)) 
        {
            vector<string> tokens;
            copy(istream_iterator<string>(ivocab1),
                 istream_iterator<string>(),
                 back_inserter(tokens));
            if (tokens.size() < 2)
                break;
            vi1.insert(make_pair(tokens[1],atoi(tokens[0].c_str())));
        }
        while (getline(ivocab2,vcbent)) 
        {
            vector<string> tokens;
            copy(istream_iterator<string>(ivocab1),
                 istream_iterator<string>(),
                 back_inserter(tokens));
            if (tokens.size() < 2)
                break;
            vi2.insert(make_pair(tokens[1],atoi(tokens[0].c_str())));
        }
        ivocab_p = !(vi1.empty() || vi2.empty());
    }

    // require no input vcb/output vcb collision in order to output vcb
    bool ovocab_p = true;
    if (!vcbpaths.empty())  
    {
        for (int i = 0; i < 2; i++) 
        {
            ovocab_p &= vocab1 != vcbpaths[i] && vocab2 != vcbpaths[i];
        }
    }
   
    ofstream osnt1(snt1.c_str()), osnt2(snt2.c_str());
  
    for (unsigned int i=0;i<filenames.size();i+=2) 
    {
        size_t lno = 0, wno = 0;
        ifstream i1(filenames[i].c_str()),i2(filenames[i+1].c_str());
        if (!i1)cerr << "WARNING: " << filenames[i] << " cannot be read.\n";
        if (!i2)cerr << "WARNING: " << filenames[i+1] << " cannot be read.\n";

        while(getline(i1,line1) && getline(i2,line2) ) 
        {
            vector<string> t1,t2;
            lno++;

            istringstream ii1(line1);
            wno = 0;
            while(ii1>>word)
            {
                wno++;
                t1.push_back(word);
                v1[word]++;
                if ( id1.find(word)==id1.end() )
                {
                    iid1.push_back(word);
                    id1[word]=static_cast<int>(iid1.size())-1;
                }
                if (ivocab_p) 
                {
                    if (vi1.find(word) == vi1.end()) {
                        cerr << "OOV, file " << filenames[i] 
                             << " line " << lno
                             << " word " << wno
                             << " \"" << word << "\"" << endl;
                        exit(1);
                    }
                }
            }

            istringstream ii2(line2);
            wno = 0;
            while(ii2>>word)
            {
                wno++;
                t2.push_back(word);
                v2[word]++;
                if ( id2.find(word)==id2.end() )
                {
                    iid2.push_back(word);
                    id2[word]=static_cast<int>(iid2.size())-1;
                }
                if (ivocab_p) 
                {
                    if (vi2.find(word) == vi2.end()) {
                        cerr << "OOV, file " << filenames[i+1] 
                             << " line " << lno
                             << " word " << wno
                             << " \"" << word << "\"" << endl;
                        exit(1);
                    }
                }
            }

            double w=1.0;
            if (i/2<weights.size())
            {
                w=weights[i/2];
            }
            if (t1.size() && t2.size())
            {
                osnt1 << w << "\n";
                for (unsigned int j=0;j<t1.size();++j)
                {
                    if (ivocab_p) {
                        osnt1 << vi1[t1[j]] << ' ';
                    } else {
                        osnt1 << id1[t1[j]] << ' ';
                    }
                }
                osnt1 << '\n';
                for (unsigned int j=0;j<t2.size();++j)
                {
                    if (ivocab_p) {
                        osnt1 << vi2[t2[j]] << ' ';
                    } else {
                        osnt1 << id2[t2[j]] << ' ';
                    }
                }
                osnt1 << '\n';
              
                osnt2 << w << "\n";
                for (unsigned int j=0;j<t2.size();++j)
                {
                    if (ivocab_p) {
                        osnt2 << vi2[t2[j]] << ' ';
                    } else {
                        osnt2 << id2[t2[j]] << ' ';
                    }
                }
                osnt2 << '\n';
                for (unsigned int j=0;j<t1.size();++j)
                {
                    if (ivocab_p) {
                        osnt2 << vi1[t1[j]] << ' ';
                    } else {
                        osnt2 << id1[t1[j]] << ' ';
                    }
                }
                osnt2 << '\n';
            } else {
                cerr << "WARNING: filtered out empty sentence (source: " 
                     << filenames[i] << " " << t1.size() <<
                    " target: " << filenames[i+1] << " " << t2.size() << ").\n";
            }
        }
    }
  
    if (ovocab_p) 
    {
        ofstream ovocab1(vocab1.c_str()), ovocab2(vocab2.c_str());
        for (unsigned int i=2;i<iid1.size();++i)
            ovocab1 << i << ' ' << iid1[i] << ' ' << v1[iid1[i]] << '\n';
        for (unsigned int i=2;i<iid2.size();++i)
            ovocab2 << i << ' ' << iid2[i] << ' ' << v2[iid2[i]] << '\n';
    }

    exit(0);
}
