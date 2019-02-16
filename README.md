# Xu
## High Performance Query Expansion Tool & Word Embedding Generator

### Introduction

Search Engines -- the software that allows a massive volume of information to be availible to anyone, from anywhere, at anytime. Arguably, the search engine has defined the information age of humanity, with its ability to provide users with relevant information corresponding to practically any query. However, difficulties arise when search engines return too many noisy results, or not enough meaningful results, each of which degrade the user experience. To combat this, most modern search engines use a technique known as query expansion, where the users query is expanded with other relevant terms to produce a more effective filter for pages.

For example, let's say a user wishes to view articles on photography. The user enters the search query "photography". If no further processing was done, the search engine would return very limited results, because it will ignore any synonyms, pluralizations or words with similar meaning to the inital query. However, if some processing were to be done on the term "photography", the query string could expand to "(photography OR photo) AND (camera OR aperture OR picture OR image OR light)". This filter for pages is much more comprehensive then the initial query, and contains simple boolean logic to further reduce noise in the search engine results.

## Problem

Now, the problem becomes programming a computer to generate these expansion strings. However, the computer doesn't understand what words are, or that they have meaning, or that words can mean similar things to other words. A simple approach (which was initially tried) was to use an open source tool WordNet to generate synonyms for the query and output a string of each synonym connected with an OR gate. This method was largely effective, yet produced extremely noisy results. However, to create more accurate and less-noisy results, a heuristic approach needed to be taken.

## Solution

There were several different approaches that were tried, however, the optimal approach is believed to be:
- Using [DataMuse](https://www.datamuse.com/api/) to get suggestions, rather than WordNet. This produced more accurate suggestions.
- Using custom trained Word2Vec model (on all of Wikipedia) to generate word vectors for each word.
- Ranking the DataMuse suggestions by their vector similarity to the initial search query, and only taking the top 20(ish) suggestions.
- Assigning each suggestion word a vector from the average vector to the word vector.
- Clustering the vectors into a number of groups, which are similar in meaning.
- Since synonyms should be combined with OR, these clusters are grouped together using OR.
- Each cluster is then grouped together using AND, which produces optimal results.

## Example Results

Here is a snippet that was generated automatically by Xu.

```
./xu queryexp wiki_model.bin prosecution

Loading model file 'wiki_model.bin'...
2562529 words loaded
Query expansion for 'prosecution':
('pursuance' OR 'prosecutors' OR 'retrial' OR 'prosecuted' OR 'case' OR 'conviction') AND ('prosecutor' OR 'prosecuting' OR 'trial' OR 'criminal' OR 'prosecute' OR 'indictment' OR 'indictments' OR 'evidentiary' OR 'attorney') AND ('prosecution')
```

## Installation Instructions

NOTE: This installation has only been tested on Ubuntu 18.04, however, should work with any Linux or Mac distribution provided the dependencies can be installed.

To make this project, the following dependencies are required: libjsoncpp-dev, libcurl4-openssl-dev.

```
sudo apt-get install libjsoncpp-dev libcurl4-openssl-dev
```

After doing this, enter the below commands. This will make a folder on your desktop containing the Xu source code, and will compile a binary.

```
cd ~/Desktop
git clone https://github.com/queensbamlab/Xu.git
cd Xu
make
./xu
```

Note that you *MUST* use Xu to compile a Word2Vec model before using, using a text document formatted with parse.pl (spaces, only periods as punctuation, numbers converted to text). Additionally, you can download a pre-trained Wikipedia model (which I used to generate the above example), from [this link]().


 



