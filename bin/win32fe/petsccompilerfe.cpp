/* $Id: petsccompilerfe.cpp,v 1.9 2001/04/17 21:17:17 buschelm Exp $ */
#include <stdlib.h>
#include <Windows.h>
#include "petsccompilerfe.h"

using namespace PETScFE;

#define UNKNOWN '*'

compiler::compiler() {
  compileoutflag = linkoutflag = "-o ";

  OutputFlag = compilearg.end();

  OptionTags = "DILchlo";

  Options['D'] = &compiler::FoundD;
  Options['I'] = &compiler::FoundI;
  Options['L'] = &compiler::FoundL;
  Options['c'] = &compiler::Foundc;
  Options['h'] = &compiler::Foundhelp;
  Options['l'] = &compiler::Foundl;
  Options['o'] = &compiler::Foundo;
  Options[UNKNOWN] = &compiler::FoundUnknown;
}

void compiler::Parse(void) {
  tool::Parse();
  LI i = arg.begin();
  compilearg.push_front(*i++);
  arg.pop_front();
  while (i != arg.end()) {
    string temp = *i;
    if (temp[0]!='-') {
      FoundFile(i);
    } else {
      char flag = temp[1];
      if (OptionTags.find(flag)==string::npos) {
        (this->*Options[UNKNOWN])(i);
      } else {
        (this->*Options[flag])(i);
      }
    }
    i++;
    arg.pop_front();
  }
}

void compiler::Execute(void) {
  tool::Execute();
  if (!helpfound) {
    /* Find location of system libraries and headers */
    string dir = compilearg.front();

    /* First check if a full path was specified with --use */
    string::size_type n = dir.find(":");
    if (n==string::npos) {
      char tmppath[MAX_PATH],*tmp;
      int length = MAX_PATH*sizeof(char);
      string extension = ".exe";
      if (SearchPath(NULL,dir.c_str(),extension.c_str(),length,tmppath,&tmp)) {
        dir = (string)tmppath;
      } else {
        string compiler=compilearg.front();
        cerr << endl << "Error: win32fe: Compiler Not Found: ";
        cerr << compiler << endl;
        cerr << "\tSpecify the complete path to ";
        cerr << compiler;
        cerr << " with --use" << endl;
        cerr << "\tUse --help for more information on win32fe options." << endl << endl;
        return;
      }
    }

    /* Compiler is located in dir/bin/compiler.exe */
    dir = dir.substr(0,dir.find_last_of("\\",dir.find_last_of("\\")-1)+1);
    /* System headers are in dir/include */
    arg.push_back("-I" + dir + "include");
    LI i = arg.end();
    FoundI(--i);
    arg.pop_back();

    /* System libraries are in dir/lib */ 
    arg.push_back("-L" + dir + "lib");
    i = arg.end();
    FoundL(--i);
    arg.pop_back();

    FixOutput();
    /* Determine if we are compiling, or linking */ 
    i=linkarg.begin();
    string temp = *i;
    if (temp == "-c") {
      Compile();
    } else {
      Link();
    }
  }
}

void compiler::Help(void) {
  tool::Help();
  cout << "For compilers:" << endl;
  cout << "  win32fe will map the following <tool options> to their native options:" << endl;
  cout << "    -c:          Compile Only, generates an object file with .o extension" << endl;
  cout << "    -l<library>: Link the file lib<library>.lib" << endl;
  cout << "    -o <file>:   Output=<file> context dependent" << endl;
  cout << "    -D<macro>:   Define <macro>" << endl;
  cout << "    -I<path>:    Add <path> to the include path" << endl;
  cout << "    -L<path>:    Add <path> to the link path" << endl;
  cout << "    -help:       <tool> specific help for win32fe" << endl << endl;
  cout << "Ex: win32fe cl -Zi -c foo.c --verbose -Iinclude" << endl << endl;
  cout << "=========================================================================" << endl << endl;
}

void compiler::Compile(void) {
  int n;
  LI i = compilearg.begin();
  string compile = *i++;
  Merge(compile,compilearg,i);

  /* Get the current working directory */
  string cwd;
  char directory[MAX_PATH];
  int length=MAX_PATH*sizeof(char);
  GetCurrentDirectory(length,directory);
  cwd=(string)directory;
  GetShortPath(cwd);

  /* Execute each compilation one at a time */ 
  for (i=file.begin();i!=file.end();i++) {
    string outfile = *i;

    if (OutputFlag==compilearg.end()) {
      /* Make default output a .o not a .obj */
      n = outfile.find_last_of(".");
      outfile = outfile.substr(0,n) + ".o";
    } else {
      /* remove output file from compilearg list */
      outfile = *OutputFlag;
      compilearg.erase(OutputFlag);
      OutputFlag = compilearg.end();
      LI ii = compilearg.begin();
      compile = *ii++;
      Merge(compile,compilearg,ii);
    }

    /* Make sure output directory exists */ 
    string::size_type n = outfile.find_last_of("\\");
    if (n!=string::npos) {
      string dir = outfile.substr(0,n);
      if (GetShortPath(dir)) {
        outfile = dir + outfile.substr(n);
      } else {
        cerr << "Error: win32fe: Output Directory Not Found: ";
        cerr << outfile.substr(0,n) << endl;
        return;
      }
    }
    outfile = compileoutflag + outfile;

    /* Concatenate the current directory with the file name if the file is local */
    string filename = *i;
    n = filename.find(":");
    if (n==string::npos) {
      n = filename.find_last_of("\\");
      if (n!=string::npos) {
        if (!GetShortPath(filename)) {
          cerr << "Error: win32fe: Input File Not Found: " << *i << endl;
          return;
        }
      }
      filename = cwd + "\\" + filename;
    }
    string compileeach = compile + " " + outfile + " " + filename;
    if (verbose) cout << compileeach << endl;
    system(compileeach.c_str());
  }
  return;
}

void compiler::Link(void) {
  LI i = compilearg.begin();
  string link = *i++;
  Merge(link,compilearg,i);
  i = file.begin();
  Merge(link,file,i);
  i = linkarg.begin();
  Merge(link,linkarg,i);
  if (verbose) cout << link << endl;
  system(link.c_str());
}

void compiler::FoundD(LI &i) {
  string temp = *i;
  ProtectQuotes(temp);
  compilearg.push_back(temp);
}

void compiler::FoundI(LI &i) {
  string shortpath = i->substr(2);
  ReplaceSlashWithBackslash(shortpath);
  if (GetShortPath(shortpath)) {
    shortpath = "-I"+shortpath;
    compilearg.push_back(shortpath);
  } else {
    cerr << "Warning: win32fe Include Path Not Found: " << i->substr(2) << endl;
  }
}

void compiler::FoundL(LI &i) {
  string shortpath =i->substr(2);
  ReplaceSlashWithBackslash(shortpath);
  if (GetShortPath(shortpath)) {
    shortpath = "-L"+shortpath;
    linkarg.push_back(shortpath);
  } else {
    cerr << "Warning: win32fe Library Path Not Found:" << i->substr(2) << endl;
  }
}

void compiler::Foundc(LI &i) {
  string temp = *i;
  if (temp=="-c") {
    compilearg.push_back(temp);
    linkarg.pop_front();
    linkarg.push_front(temp);
  } else {
    compilearg.push_back(temp);
  }
}

void compiler::Foundhelp(LI &i) {
  if (*i=="-help") {
    helpfound = -1;
  }
}

void compiler::Foundl(LI &i) { 
  string temp = *i;
  file.push_back("lib" + temp.substr(2) + ".lib");
} 

void compiler::Foundo(LI &i) {
  if (*i == "-o") {
    i++;
    arg.pop_front();
    string temp = *i;
    ReplaceSlashWithBackslash(temp);
    compilearg.push_back(temp);
    /* Set Flag then fix later based on compilation or link */
    OutputFlag = --compilearg.end();
  } else {
    compilearg.push_back(*i);
  }
}   

void compiler::FoundUnknown(LI &i) {
  string temp = *i;
  compilearg.push_back(temp);
}

void compiler::FixOutput(void) {
  if (OutputFlag!=compilearg.end()) {
    string outfile = *OutputFlag;
    compilearg.erase(OutputFlag);
    if (linkarg.front()=="-c") {
      compilearg.push_back(outfile);
      OutputFlag = --compilearg.end();
    } else {
      outfile = linkoutflag + outfile;
      linkarg.push_front(outfile);
      OutputFlag = --linkarg.begin();
    }
  }
}
