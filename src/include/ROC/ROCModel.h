#ifndef ROCMODEL_H
#define ROCMODEL_H

#include "../base/Model.h"
#include "ROCParameter.h"

class ROCModel : public Model
{
    private:
		ROCParameter *parameter;
		bool withPhi;

		double calculateLogLikelihoodPerAAPerGene(unsigned numCodons, int codonCount[], double mutation[], double selection[], double phiValue);
		double calculateMutationPrior(std::string grouping, bool proposed = false); // TODO add to FONSE as well? // cedric
		void obtainCodonCount(SequenceSummary *seqsum, std::string curAA, int codonCount[]);

    public:
		//Constructors & Destructors:
		ROCModel(bool _withPhi = false);
		virtual ~ROCModel();



		//Likelihood Ratio Functions:
		virtual void calculateLogLikelihoodRatioPerGene(Gene& gene, unsigned geneIndex, unsigned k,
					double* logProbabilityRatio);
		virtual void calculateLogLikelihoodRatioPerGroupingPerCategory(std::string grouping, Genome& genome,
					double& logAcceptanceRatioForAllMixtures);
		virtual void calculateLogLikelihoodRatioForHyperParameters(Genome &genome, unsigned iteration,
					std::vector <double> &logProbabilityRatio);


		//Initialization and Restart Functions:
		virtual void initTraces(unsigned samples, unsigned num_genes);
		virtual void writeRestartFile(std::string filename);



		//Category Functions:
		virtual double getCategoryProbability(unsigned i);
		virtual unsigned getMutationCategory(unsigned mixture);
		virtual unsigned getSelectionCategory(unsigned mixture);
		virtual unsigned getSynthesisRateCategory(unsigned mixture);
		virtual std::vector<unsigned> getMixtureElementsOfSelectionCategory(unsigned k);



		//Group List Functions:
		virtual unsigned getGroupListSize();
		virtual std::string getGrouping(unsigned index);



		//StdDevSynthesisRate Functions:
		virtual double getStdDevSynthesisRate(unsigned selectionCategory, bool proposed = false);
		virtual double getCurrentStdDevSynthesisRateProposalWidth();
		virtual void updateStdDevSynthesisRate();



		//Synthesis Rate Functions:
		virtual double getSynthesisRate(unsigned index, unsigned mixture, bool proposed = false);
		virtual void updateSynthesisRate(unsigned i, unsigned k);



		//Iteration Functions:
		virtual unsigned getLastIteration();
		virtual void setLastIteration(unsigned iteration);



		//Trace Functions:
		virtual void updateStdDevSynthesisRateTrace(unsigned sample);
		virtual void updateSynthesisRateTrace(unsigned sample, unsigned i) ;
		virtual void updateMixtureAssignmentTrace(unsigned sample, unsigned i) ;
		virtual void updateMixtureProbabilitiesTrace(unsigned sample);
		virtual void updateCodonSpecificParameterTrace(unsigned sample, std::string grouping);
		virtual void updateHyperParameterTraces(unsigned sample);
		virtual void updateTracesWithInitialValues(Genome &genome);



		//Adaptive Width Functions:
		virtual void adaptStdDevSynthesisRateProposalWidth(unsigned adaptiveWidth);
		virtual void adaptSynthesisRateProposalWidth(unsigned adaptiveWidth);
		virtual void adaptCodonSpecificParameterProposalWidth(unsigned adaptiveWidth);
		virtual void adaptHyperParameterProposalWidths(unsigned adaptiveWidth);



		//Other Functions:
		virtual void proposeCodonSpecificParameter();
		virtual void proposeHyperParameters();
		virtual void proposeSynthesisRateLevels();

		virtual unsigned getNumPhiGroupings() ;
		virtual unsigned getMixtureAssignment(unsigned index);
		virtual unsigned getNumMixtureElements() ;
		virtual unsigned getNumSynthesisRateCategories();

		virtual void setNumPhiGroupings(unsigned value);
		virtual void setMixtureAssignment(unsigned i, unsigned catOfGene);
		virtual void setCategoryProbability(unsigned mixture, double value);

		virtual void updateCodonSpecificParameter(std::string grouping);
		virtual void updateGibbsSampledHyperParameters(Genome &genome);
		virtual void updateAllHyperParameter();
		virtual void updateHyperParameter(unsigned hp);

		void simulateGenome(Genome &genome);
		virtual void printHyperParameters();
		void setParameter(ROCParameter &_parameter);
		virtual double calculateAllPriors();
		void calculateCodonProbabilityVector(unsigned numCodons, double mutation[], double selection[], double phi, double codonProb[]);
		virtual void getParameterForCategory(unsigned category, unsigned param, std::string aa, bool proposal, double* returnValue);


		//ROC Specific Functions:
		double getAphi(unsigned index, bool proposed = false);
		double getSepsilon(unsigned index) ;
		double getCurrentAphiProposalWidth(unsigned index);
		void updateAphi(unsigned index);
		void updateAphiTrace(unsigned index, unsigned sample);
		void updateSepsilonTrace(unsigned index, unsigned sample);
		void adaptAphiProposalWidth(unsigned adaptiveWidth);



		//R Section:
#ifndef STANDALONE
		std::vector<double> CalculateProbabilitiesForCodons(std::vector<double> mutation, std::vector<double> selection,
							double phi);
#endif //STANDALONE

    protected:
};

#endif // ROCMODEL_H
