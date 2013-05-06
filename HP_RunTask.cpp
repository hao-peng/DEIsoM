/*
Run the analysis for one gene
*/

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <list>
#include <boost/filesystem.hpp>
#include "HP_Model.h"

using namespace std;

list<HP_Param> parseArg(int argc, char **argv)  {
	HP_Param param;
	list<string> gffs;
	list<string> geneIDs;
	bool errorFlag = false;
	// parse options
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--gene-ids")==0 && i+1<argc) {
			char *id = strtok(argv[++i], ",");
			do {
				geneIDs.push_back(id);
			} while((id = strtok(NULL, ",")) != NULL);
		} else if (strcmp(argv[i], "--gffs")==0 && i+1<argc) {
			char *gff = strtok(argv[++i], ",");
			do {
				if (!boost::filesystem::exists(gff)){
					errorFlag = true;
					cerr << "Error: cannot access GFF file \"" << gff << "\"." << endl;
				}
				gffs.push_back(gff);
			} while ((gff = strtok(NULL, ",")) != 0);
		} else if (strcmp(argv[i], "--bams")==0 && i+1<argc) {
			char *filename = strtok(argv[++i], ",");
			do {
				param.bams.push_back(filename);
				if (!boost::filesystem::exists(filename)) {
					errorFlag = true;
					cerr << "Error: cannot access BAM file \"" << filename << "\"." << endl;
				}
			} while ((filename = strtok(NULL, ",")) != NULL);
		} else if (strcmp(argv[i], "--output")==0 && i+1<argc) {
			param.outputDir = string(argv[++i]);
			try {
				boost::filesystem::create_directories(param.outputDir);
			} catch (const boost::filesystem::filesystem_error& e) {
				errorFlag = true;
				cerr << "Error: cannot write to directory \"" << argv[i] << "\"." << endl;
			}
		} else if (strcmp(argv[i], "--min-read")==0 && i+1<argc) {
			param.minRead = atoi(argv[++i]);
			if (param.minRead == 0) {
				errorFlag = true;
				cerr << "Error: invalid minimum number of read \"" << argv[i] << "\"." << endl;
			}
		} else if (strcmp(argv[i], "--read-len")==0 && i+1<argc) {
			param.readLen = atoi(argv[++i]);
			if (param.readLen == 0) { 
				errorFlag = true;
				cerr << "Error: invalid read length \"" << argv[i] << "\"." << endl;
			}
		} else if (strcmp(argv[i], "--overhang-len")==0 && i+1<argc) {
			param.overhangLen = atoi(argv[++i]);
			if (param.overhangLen == 0) { 
				errorFlag = true;
				cerr << "Error: invalid overhang length \"" << argv[i] << "\"." << endl;
			}
		} else if (strcmp(argv[i], "--paired-end") == 0 && i+2<argc) {
			param.isSingleEnd = 0;
			param.meanInsertedLen = atof(argv[++i]); // now it does not check for error
			param.stdInsertedLen = atof(argv[++i]); // not it does not check for error
		} else if (strcmp(argv[i], "--in-iter")==0 && i+1<argc) {
			param.numInIters = atoi(argv[++i]);
			if (param.numInIters == 0) { 
				errorFlag = true;
				cerr << "Error: invalid number of inner iterations \"" << argv[i] << "\"." << endl;
			}
		} else if (strcmp(argv[i], "--out-iter")==0 && i+1<argc) {
			param.numOutIters = atoi(argv[++i]);
			if (param.numOutIters == 0) { 
				errorFlag = true;
				cerr << "Error: invalid number of outer iterations \"" << argv[i] << "\"." << endl;
			}
		} else {
			cerr << "Error: cannot recognize option " << i << " : \"" << argv[i] << "\"." << endl;
			errorFlag = true;
		}
	}
	if (gffs.size() != geneIDs.size()) {
		errorFlag = true;
		cerr << "Number of gene IDs does not match number of GFF files" << endl;
	}
	// check required options
	if (!errorFlag) {
		if (geneIDs.empty()) {
			errorFlag = true;
			cerr << "Error: Gene ID is required" << endl;
		}
		if (param.readLen == 0) {
			errorFlag = true;
			cerr << "Error: read length is required" << endl;
		}
		if (param.bams.empty()) {
			errorFlag = true;
			cerr << "Error: BAM filenames are required" << endl;
		}
		if (param.outputDir.empty()) {
			errorFlag = true;
			cerr << "Error: Output directory is required" << endl;
		}
	}
	// print usage
	else {
		cerr << "Usage: " << argv[0] << " <options>" << endl;
		cerr << "Options:" << endl;
		cerr << "--gene-ids <gene ID1>,<gene ID2>,..." << endl;
		cerr << "--gffs <GFF filename 1>,<GFF filename 2>,..." << endl;
		cerr << "--bams <BAM filename 1>,<BAM filename 2>,..." << endl;	
		cerr << "--output <output directory>" << endl;
		cerr << "--min-read <minimum number of reads>" << endl;
		//cerr << "--read-len <read length>" << endl;
		cerr << "--overhang-len <overhang length>" << endl;	
		cerr << "--paired-end <mean> <std>" << endl;	
		cerr << "--in-iter <# of inner iters>" << endl;	
		cerr << "--out-iter <# of outer iters>" << endl;	
	}

	if (errorFlag)
		exit(1);
	
	list<HP_Param> params;
	for (list<string>::iterator ii = geneIDs.begin(), ij = gffs.begin();
		 ii != geneIDs.end() && ij != gffs.end();
		 ii++, ij++) {
		param.geneID = *ii;
		param.gff = *ij;
		params.push_back(param);
	}
	return params;
}

int main(int argc, char **argv) {
	list<HP_Param> params = parseArg(argc, argv);
	for (list<HP_Param>::iterator ii = params.begin();
		 ii != params.end(); ii++) {
		cerr << "------------------------------------------" << endl
			<< "Computing:" << endl << ii->toString() << endl;
		HP_Model model(*ii);
		model.preprocessing();
	}
	
	return 0;
}
