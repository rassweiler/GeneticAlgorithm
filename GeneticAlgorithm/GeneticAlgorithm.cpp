/*
=====================================================================
|	Author: Kyle Rassweiler											|
|	Date: 2015-12-04												|
|	License: MIT													|
|	Description: Evolution algorithm example.						|
|	Source: 														|
|																	|
|	Rewritten from Processing example: 								|
|	http://natureofcode.com/book/chapter-9-the-evolution-of-code/	|
=====================================================================
*/

#include "stdafx.h"

class DNA{
public:
	//Create from random
	DNA(int iLength){
		std::random_device rdt;
		std::mt19937 mtt(rdt());

		//random value within ascii characters
		std::uniform_int_distribution<int> distt(32, 128);

		//Populate this new array with random char values
		for (int i = 0; i < iLength; ++i){
			cGenes.push_back((char)distt(mtt));
		}

		//Init fitness and mutation values
		fFitness = 0.0f;
		fMutation = 0.01f;
	};

	//Create from parents
	DNA(std::shared_ptr<DNA> parent1, std::shared_ptr<DNA> parent2){
		std::random_device rdt;
		std::mt19937 mtt(rdt());

		//random value within gene size
		std::uniform_int_distribution<int> distt(0, parent1->GetGeneSize());

		/*Set a midpoint for deciding which parent to take gene from.
		Alternatively this could be done by doing 50/50 chance
		on each gene */
		int m = distt(mtt);

		//Begin filling cGenes array TODO: Ensure genes pushed in correct order
		for (int i = 0; i < parent1->GetGeneSize(); ++i){
			if (i > m){
				cGenes.push_back(parent1->GetGeneAt(i));
			}
			else{
				cGenes.push_back(parent2->GetGeneAt(i));
			}
		}

		//Initiate fitness and mutation
		fFitness = 0.0f;
		fMutation = 0.01f;
	};

	//Grab the private fitness value
	float GetFitness(){
		return fFitness;
	};

	//Set the fitness based on the passed target
	void SetFitness(std::string sTarget){
		if (sTarget != ""){
			int score = 0;
			for (int i = 0; i < cGenes.size(); ++i){
				if (cGenes[i] == sTarget.at(i)){
					++score;
				}
			}
			fFitness = ((float)score) / sTarget.size();
		}
	};

	//Get gene sequence as string
	std::string GetGeneString(){
		std::string temp(cGenes.begin(), cGenes.end());
		return temp;
	};

	//Get size of gene array
	int GetGeneSize(){
		return cGenes.size();
	};

	//Get gene at
	char GetGeneAt(int i){
		return cGenes[i];
	};

	//Mutate genes
	void Mutate(){
		std::random_device rdt;
		std::mt19937 mtt(rdt());

		//random value 0 - 1
		std::uniform_real_distribution<float> distt(0, 1);

		//random char value
		std::uniform_int_distribution<int> distt2(32, 128);

		for (int i = 0; i < cGenes.size(); ++i){
			float t = distt(mtt);
			if (t < fMutation){
				cGenes[i] = distt2(mtt);
			}
		}
	};
private:
	std::vector<char> cGenes;
	float fFitness;
	float fMutation;
};

int _tmain(int argc, _TCHAR* argv[])
{
	//cpp11 random, need to include <random>
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> dist(100, 1000);

	//DNA population containers
	std::vector<std::shared_ptr<DNA>> vPopulation;
	std::vector<std::shared_ptr<DNA>> vPool;

	//Get initial data
	int iGeneLength = 0;
	int iPopulationSize = dist(mt);
	std::cout << "Population Size: ";
	std::cout << iPopulationSize;
	std::cout << "\n";
	std::cout << "Type a sentence for the target: ";
	std::string sTarget;
	std::getline(std::cin, sTarget);
	std::cout << "\n";
	iGeneLength = sTarget.size();
	bool bReachedGoal = false;
	unsigned long lGeneration = 1;

	//Initiate population TODO: verify that smart pointer is created properly!
	for (int i = 0; i < iPopulationSize; ++i){
		std::shared_ptr<DNA> temp(new DNA(iGeneLength));
		vPopulation.push_back(temp);
	}

	//Begin the Circle Of Life™
	while (!bReachedGoal){

		//Print Info
		std::cout << "Generation: ";
		std::cout << lGeneration;
		std::cout << "\n";

		//Determine fitness and print highest and gene
		for (auto it = vPopulation.begin(); it != vPopulation.end(); ++it){
			(*it)->SetFitness(sTarget);
		}
		float highest = 0;
		int index = 0;
		for (auto it = vPopulation.begin(); it != vPopulation.end(); ++it){
			if ((*it)->GetFitness() > highest){
				highest = (*it)->GetFitness();
				index = it - vPopulation.begin();
			}
		}

		std::cout << "Highest Fitness: ";
		std::cout << highest*100;
		std::cout << "% with gene sequence: ";
		std::cout << vPopulation.at(index)->GetGeneString();
		std::cout << "\n";
		if (highest == 1){
			bReachedGoal = true;
		}

		//Fill out mating pool
		for (auto it = vPopulation.begin(); it != vPopulation.end(); ++it){
			int n = (*it)->GetFitness() * 100;
			for (int i = 0; i < n; ++i){

				/*Create a new shared_ptr i number of times based on
				the fitness. TODO: Verify strong ref increases */
				vPool.push_back((*it));
			}
		}

		//Clear old population (not realistic but deal)
		vPopulation.clear();

		//Mate pairs to recreate population
		std::uniform_int_distribution<int> dist2(0, vPool.size()-1);

		//Refill population to same size as it was with new DNA
		for (int i = 0; i < iPopulationSize; ++i){

			//Init iterators
			int x = 0;
			int y = 0;

			//Set random value based on size of pool container
			x = dist2(mt);
			y = dist2(mt);

			//Make sure not to use the same parent twice
			while (vPool[x] == vPool[y]){
				y = dist2(mt);
			}

			//Create new DNA based on parents
			std::shared_ptr<DNA> temp(new DNA(vPool.at(x), vPool.at(y)));

			//Add mutation
			temp->Mutate();

			//Place new DNA in population container
			vPopulation.push_back(temp);
		}

		//Increase generation
		++lGeneration;
	}
	std::getline(std::cin, sTarget);
	return 0;
}