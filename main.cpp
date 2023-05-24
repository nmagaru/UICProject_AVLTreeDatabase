/*main.cpp*/

// myDB project using AVL trees

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cassert>
#include <algorithm> //for find and count

#include "avl.h"
#include "util.h"

using namespace std;


//
// tokenize
//
// Given a string, breaks the string into individual tokens (words) based
// on spaces between the tokens.  Returns the tokens back in a vector.
//
// Example: "select * from students" would be tokenized into a vector
// containing 4 strings:  "select", "*", "from", "students".
//
vector<string> tokenize(string line)
{
  vector<string> tokens;
  stringstream  stream(line);
  string token;

  while (getline(stream, token, ' '))
  {
    tokens.push_back(token);
  }

  return tokens;
}


int main()
{
	string tablename; // = "students";

	cout << "Welcome to myDB, please enter tablename> ";
	getline(cin, tablename);

	
	// META DATA CODE //
	cout << "Reading meta-data..." << endl;
	
	vector<string> metavect;
	vector<int> columnvect;
	vector<string> columnnamevect;
	vector<string> indexednamevect;
	int indexcount = 0;
	
	// access the respective meta file
	string metafilename = tablename + ".meta";
	ifstream metadata(metafilename, ios::in | ios::binary);

	// check if file can be opened
	if (!metadata.good())
	{
		cout << "**Error: couldn't open data file '" << metafilename << "'." << endl;
		return 0;
	}
	
	string metaval;
	metadata >> metaval;
	
	// loop through each line of meta file
	while (!metadata.eof())
	{
		// check for 0/1 for indexed/non-indexed columns
		// push value into respective vectors
		if (metaval == "0")
		{
			columnnamevect.push_back(metavect.back());
			
			indexcount++;
		}
		else if (metaval == "1")
		{
			columnvect.push_back(indexcount);
			columnnamevect.push_back(metavect.back());
			indexednamevect.push_back(metavect.back());
			
			indexcount++;
		}
		
		metavect.push_back(metaval);
		
		metadata >> metaval;
	}
	
	
	// INDEX TREE CODE //
	cout << "Building index tree(s)..." << endl;
	
	// access the respective data file
	string datafilename = tablename + ".data";
	ifstream data(datafilename, ios::in | ios::binary);
	
	// check if file can be opened
	if (!data.good())
	{
		cout << "**Error: couldn't open data file '" << datafilename << "'." << endl;
		return 0;
	}
	
	string key, temp;
	streamoff value;
	int numspaces = stoi(metavect[0]); 
	int numcolumns = stoi(metavect[1]);
	int linecount = 0;
	vector<string> recorddata;
	vector<avltree<string, streamoff>> treevect;
	
	// loop through each line of data file (for line count)
	while (!data.eof())
	{
		getline(data, temp);
		linecount++;
	}
	
	// loop through column index vector
	size_t numindices = columnvect.size();
	for (size_t i = 0; i < numindices; i++)
	{
		avltree<string, streamoff> indextree;
		
		// loop number of lines times
		for (int j = 0; j < linecount - 1; j++)
		{
			value = j * numspaces;
			// call GetRecord to get record data at each line
			recorddata = GetRecord(tablename, value, numcolumns);
			key = recorddata[columnvect[i]];
			
			// insert the indexed column value into avl tree for searching
 			indextree.insert(key, value);
		}
		
		// push indexed column tree into tree vector to track each column's data
 		treevect.push_back(indextree);
	}
	
	// loop through tree vector
	size_t treevectsize = treevect.size(); 
	for (size_t i = 0; i < treevectsize; i++)
	{
		cout << "Index column: " << columnnamevect[columnvect[i]] << endl;
		cout << "\tTree size: " << treevect[i].size() << endl;
		cout << "\tTree height: " << treevect[i].height() << endl;
	}

	// QUERY CODE //
	//
	// Main loop to input and execute queries from the user:
	//
	string query, columnname1, columnname2, searchval;
	streamoff searchpos;
	vector<string> searchvect;
	vector<string>::iterator finditerator;
	int index;
	
	// continuous while loop until user query exit
	while (true)
	{	
		cout << endl;
		cout << "Enter query> ";
		getline(cin, query);
		
		if (query == "exit")
			break;
		
		// call tokenize to push query into a vector
		vector<string> tokens = tokenize(query);
		// declare boolean to check if query value was found or not
		bool checkval = false;
		
		// check if first query word is select
		if (tokens.front() != "select")
		{
			cout << "Unknown query, ignored...\n";
			continue;
		}
	
		// these erase statements are used for:
		// - checking correct number of query words
		// - progressing through the query vector
		tokens.erase(tokens.begin());

		// check if selection column is valid (column name or *)
		columnname1 = tokens.front();
		if ( (columnname1 != "*") &&
			 (count(columnnamevect.begin(), columnnamevect.end(), columnname1) == 0) )
		{
			cout << "Invalid select column, ignored...\n";
			continue;	
		}

		tokens.erase(tokens.begin());

		// check if third query word is from
		if (tokens.front() != "from")
		{
			cout << "Invalid select query, ignored...\n";
			continue;
		}

		tokens.erase(tokens.begin());

		// check if the tablename is valid (current file name)
		if (tokens.front() != tablename)
		{
			cout << "Invalid table name, ignored...\n";
			continue;
		}

		tokens.erase(tokens.begin());
		
		// check if the fifth query word is where
		if (tokens.front() != "where")
		{
			cout << "Invalid select query, ignored...\n";
			continue;
		}

		tokens.erase(tokens.begin());
		
		// check if search column is valid (meta data columns)
		columnname2 = tokens.front();
		if (count(columnnamevect.begin(), columnnamevect.end(), columnname2) == 0) 
		{
			cout << "Invalid where column, ignored...\n";
			continue;	
		}
		
		tokens.erase(tokens.begin());
		
		// check if seventh query word is =
		if (tokens.front() != "=") 
		{
			cout << "Invalid select query, ignored...\n";
			continue;	
		}
		
		tokens.erase(tokens.begin());
		
		searchval = tokens.front();
		
		tokens.erase(tokens.begin());
		
		// check if the query has too many words in it
		if (!tokens.empty())
		{
			cout << "Query too long, ignored...\n";
			continue;
		}
		
		// compare if the search column is indexed or not
		if (count(indexednamevect.begin(), indexednamevect.end(), columnname2) != 0)
		{
			// use find multiple times to find the index of a given column name in a vector
			finditerator = find(indexednamevect.begin(), indexednamevect.end(), columnname2);
			index = distance(indexednamevect.begin(), finditerator);
			
			// check respective avl tree if search value exists in tree
			if (treevect[index].search(searchval) != nullptr)
			{
				// set boolean true since value was found
				checkval = true;

				searchpos = *treevect[index].search(searchval);
				searchvect = GetRecord(tablename, searchpos, stoi(metavect[1]));
				
				// compare if a column or all columns are specified
				if (columnname1 == "*")
				{
					// loop through the search record data vector
					size_t searchvectsize = searchvect.size();
					for (size_t i = 0; i < searchvectsize; i++)
						cout << columnnamevect[i] << ": " << searchvect[i] << endl;
				}
				else
				{
					finditerator = find(columnnamevect.begin(), columnnamevect.end(), columnname1);
					index = distance(columnnamevect.begin(), finditerator);

					cout << columnnamevect[index] << ": " << searchvect[index] << endl;
				}
			}
			
		}
		else
		{
			vector<streamoff> posvect;
			vector<string> noindexrecorddata;
			finditerator = find(columnnamevect.begin(), columnnamevect.end(), columnname2);
			index = distance(columnnamevect.begin(), finditerator);
			
			// call LinearSearch to search for search value in non-indexed columns
			posvect = LinearSearch(tablename, numspaces, numcolumns, searchval, index);
			
			// loop trough position vector
			size_t posvectsize = posvect.size();
			for (size_t i = 0; i < posvectsize; i++)
			{
				// set boolean true since value was found
				checkval = true;
				
				// call GetRecord to get non-indexed record data at each line
				noindexrecorddata = GetRecord(tablename, posvect[i], numcolumns);
				
				// compare if a column or all columns are specified
				if (columnname1 == "*")
				{
					// loop through the non-indexed search record data vector
					for (size_t l = 0; l < noindexrecorddata.size(); l++)
						cout << columnnamevect[l] << ": " << noindexrecorddata[l] << endl;
				}
				else
				{
					finditerator = find(columnnamevect.begin(), columnnamevect.end(), columnname1);
					index = distance(columnnamevect.begin(), finditerator);

					cout << columnnamevect[index] << ": " << noindexrecorddata[index] << endl;
				}
			}
		}
		
		// check if search value was not found
		if (!checkval)
				cout << "Not found...\n";
	}

	//
	// done:
	//
	return 0;
}
