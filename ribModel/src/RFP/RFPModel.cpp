#include "../include/RFP/RFPModel.h"

RFPModel::RFPModel() : Model()
{
	//ctor
}


RFPModel::~RFPModel()
{
	//dtor
	//TODO: call Parent's deconstructor
}


RFPModel::RFPModel (const RFPModel& other)
{
	//TODO: complete implimentation
}


double RFPModel::calculateLogLikelihoodPerCodonPerGene(double currAlpha, double currLambdaPrime, unsigned currRFPObserved, 
		unsigned currNumCodonsInMRNA, double phiValue)
{
	//TODO: is lgamma the correct function to use here? Any reason to make the math cleaner?
	double logLikelihood = (std::log(std::lgamma((currNumCodonsInMRNA * currAlpha) + currRFPObserved)) - std::log(std::lgamma(currNumCodonsInMRNA * currAlpha)))
		+ (currRFPObserved * (std::log(phiValue) + std::log(currLambdaPrime + phiValue))) + ((currNumCodonsInMRNA * currAlpha) * (std::log(currLambdaPrime) + 
					std::log(currLambdaPrime + phiValue)));

	return logLikelihood;
}


void RFPModel::setParameter(RFPParameter &_parameter)
{
	parameter = &_parameter;
}


void RFPModel::calculateLogLiklihoodRatioPerGene(Gene& gene, int geneIndex, unsigned k, double* logProbabilityRatio)
{
	//TODO: Many functions used here are unimplimented as of now
	double logLikelihood = 0.0;
	double logLikelihood_proposed = 0.0;

	unsigned alphaCategory = parameter->getMutationCategory(k);
	unsigned lambdaPrimeCategory = parameter->getSelectionCategory(k);
	unsigned synthesisRateCategory = parameter->getSynthesisRateCategory(k);


	double phiValue = parameter->getSynthesisRate(geneIndex, synthesisRateCategory, false);
	double phiValue_proposed = parameter->getSynthesisRate(geneIndex, synthesisRateCategory, true);
    

	for (unsigned index = 0; index < 61; index++) //number of codons, without the stop codons
	{
		std::string codon = SequenceSummary::IndexToCodon(index, false); //may not need

		double currAlpha = getAlphaByCodonIndex(index, alphaCategory, false); //getParamForCategory
		double currLambdaPrime = getLambdaPrimeByCodonIndex(index, lambdaPrimeCategory, false); //getParamForCategory
		unsigned currRFPObserved = gene.geneData.getRFPObserved(index);
		unsigned currNumCodonsInMRNA = gene.geneData.getNumCodonsInMRNA(index);

		logLikelihood += calculateLogLikelihoodPerCodonPerGene(currAlpha, currLambdaPrime, currRFPObserved, currNumCodonsInMRNA, phiValue);
		logLikelihood_proposed += calculateLogLikelihoodPerCodonPerGene(currAlpha, currLambdaPrime, currRFPObserved, currNumCodonsInMRNA, phiValue_proposed);
	}

	double sPhi = parameter->getSphi(false);
	double logPhiProbability = std::log(ROCParameter::densityLogNorm(phiValue, (-(sPhi * sPhi) / 2), sPhi));
	double logPhiProbability_proposed = std::log(Parameter::densityLogNorm(phiValue_proposed, (-(sPhi * sPhi) / 2), sPhi));
	double currentLogLikelihood = (logLikelihood + logPhiProbability);
	double proposedLogLikelihood = (logLikelihood_proposed + logPhiProbability_proposed);

	logProbabilityRatio[0] = (proposedLogLikelihood - currentLogLikelihood) - (std::log(phiValue) - std::log(phiValue_proposed));
	logProbabilityRatio[1] = currentLogLikelihood - std::log(phiValue_proposed);
	logProbabilityRatio[2] = proposedLogLikelihood - std::log(phiValue);
}


void RFPModel::calculateLogLikelihoodRatioPerGroupingPerCategory(std::string grouping, Genome& genome, double& logAcceptanceRatioForAllMixtures)
{
	double likelihood = 0.0;
	double likelihood_proposed = 0.0;
    unsigned index = SequenceSummary::CodonToIndex(grouping);
    for (unsigned i = 0; i < genome.size(); i++)
    {
        Gene gene = genome.getGene(i);
        // which mixture element does this gene belong to
        unsigned mixtureElement = parameter->getMixtureAssignment(i);
        // how is the mixture element defined. Which categories make it up
        unsigned alphaCategory = parameter->getMutationCategory(mixtureElement);
        unsigned lambdaPrimeCategory = parameter->getSelectionCategory(mixtureElement);
        unsigned synthesisRateCategory = parameter->getSynthesisRateCategory(mixtureElement);
        // get non codon specific values, calculate likelihood conditional on these
        double phiValue = parameter->getSynthesisRate(i, synthesisRateCategory, false);
        unsigned currRFPObserved = gene.geneData.getRFPObserved(index);
        unsigned currNumCodonsInMRNA = gene.geneData.getNumCodonsInMRNA(index);
        
        
        double currAlpha = getAlphaByCodonIndex(index, alphaCategory, false);
        double currLambdaPrime = getLambdaPrimeByCodonIndex(index, lambdaPrimeCategory, false);
        
        double propAlpha = getAlphaByCodonIndex(index, alphaCategory, true);
        double propLambdaPrime = getLambdaPrimeByCodonIndex(index, lambdaPrimeCategory, true);
        
        logLikelihood += calculateLogLikelihoodPerCodonPerGene(currAlpha, currLambdaPrime, currRFPObserved, currNumCodonsInMRNA, phiValue);
        logLikelihood_proposed += calculateLogLikelihoodPerCodonPerGene(propAlpha, propLambdaPrime, currRFPObserved, currNumCodonsInMRNA, phiValue);
        
        logAcceptanceRatioForAllMixtures = likelihood_proposed - likelihood;
        
    }
}