#include<dirent.h>
#include<filesystem>
#include<fstream>
#include<iostream>
#include<string>
#include<vector>

using namespace std;

void get_filenames(vector<string>&);
void set_title(vector<string>&, vector<string>&);
string escape_quotes(string);
void print(vector<string>, vector<string>);
void set_json(vector<string>, vector<string>, string);

const string src = "C:\\path\\to\\source\\files\\";
const string dest = "C:\\path\\to\\destination\\files\\filename.json";

int main()
{
    vector<string> fname;
    vector<string> title;
    string desc = "DESCRIPTIVE NAME OF CODE";

    get_filenames(fname);

    set_title(fname,title);

    //print(fname,title);
    set_json(fname,title,desc);

    return EXIT_SUCCESS;

}

void get_filenames(vector<string>& vstr)
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(src.c_str())) != NULL)
    {
        cout << "FOUND FILES" << endl;

        while ((ent = readdir (dir)) != NULL)
        {
            if(ent->d_name != "." && ent->d_name != "..")
            vstr.push_back(ent->d_name);

            cout << ent->d_name << endl;
        }
        closedir (dir);
    }
    else
        cout << "NO FILES FOUND" << endl;
}

void set_title(vector<string>& vfile, vector<string>& vtitle)
{
    string temp = " ";
    for(int i = 0; i < vfile.size(); i++)
    {
        if(i < 2)
            vfile.erase(vfile.begin());
    }

    for(int i = 0; i < vfile.size(); i++)
    {
        temp = vfile[i];

        replace(temp.begin(),temp.end(),'_',' ');

        vtitle.push_back(temp);
    }

}

string escape_quotes(string str)
{
    string escaped = str;
    int str_ct = 0;
    int str_sz = escaped.size();

    while(str_ct < str_sz)
    {
        if(escaped[str_ct] == '"')
        {
            cout << "found quote" << endl;
            escaped.insert(str_ct,"\\");
            str_sz = escaped.size();
            str_ct += 1;
        }

        if(escaped[str_ct] == '\t')
        {
            cout << "found tab" << endl;
            escaped.erase(str_ct,1);
            escaped.insert(str_ct,"    ");
            str_sz = escaped.size();
        }
        str_ct+=1;
    }

    return escaped;
}


void print(vector<string> vfile, vector<string> vtitle)
{
    cout << "\t\tFILENAMES:\n\n";
    for(int i = 0; i < vfile.size(); i++)
    {
        cout << "\t\t" << vfile[i] << endl;
    }
    cout << "\n\n\t\tTITLES:\n\n";
    for(int i = 0; i < vtitle.size(); i++)
    {
        cout << "\t\t" << vtitle[i] << endl;
    }
}


void set_json(vector<string> vfile, vector<string> vtitle, string str)
{
    int count = 0;
    ifstream fin;
    ofstream fout;
    string outfile = dest;
    fout.open(outfile);

    string line = " ";

    string json_header = "{\n    \".source.ext\":\n    {";
    string json_title = "\n        \"";
    string json_prefix = "\":\n        {\n            \"prefix\":[\"";
    string json_body_begin = "\"],\n            \"body\":[\"";
    string json_body_ln = "\",\n                    \"";
    string json_description = "\"\n        ],\n        \"description\":\"";
    string json_rec_foot = "\"\n        }";
    string json_footer = "\n    }\n}";

    string border = " * ---------------------------------------------------------------------------------------------";
    string mod_h = " *  MOD     DATE            ENGINEER        COMMENTS";
    string mod_b = " *  000     $CURRENT_MONTH/$CURRENT_DATE/$CURRENT_YEAR      CommWx FSI      Script Creation";
    string author = " *  Author:         ${1:xx000000}";
    string domain = " *  Domain:         ${2:y0000}";
    string creation = " *  Creation Date:  $CURRENT_MONTH/$CURRENT_DATE/$CURRENT_YEAR $CURRENT_HOUR:$CURRENT_MINUTE:$CURRENT_SECOND";

    fout << json_header;

    for(int i = 0; i < vfile.size(); i++)
    {
        count = 0;
        fin.open(src + vfile[i]);

        if(i > 0)
            fout << ",\n";

        fout << json_title << vtitle[i];
        fout << json_prefix << vtitle[i];
        fout << json_body_begin;

        getline(fin,line);
        count++;
        line = escape_quotes(line);
        fout << line << json_body_ln;

        while(!fin.eof())
        {
            getline(fin,line);
            count++;

            if(count == 2 || count == 6)
                fout << border << json_body_ln;
            else if(count == 7)
                fout << author << json_body_ln;
            else if(count == 8)
                fout << domain << json_body_ln;
            else if(count == 9)
                fout << creation << json_body_ln;
            else if(count == 10)
            {
                fout << border << json_body_ln;
                fout << mod_h << json_body_ln;
                fout << border << json_body_ln;
                fout << mod_b << json_body_ln;
            }
            else
            {
              line = escape_quotes(line);
              fout << line << json_body_ln;
            }
        }
        fin.close();

        fout << json_description << str;

        fout << json_rec_foot;

    }

    fout << json_footer;
    fout.close();
}