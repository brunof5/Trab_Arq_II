#include "Alu.cpp"

class ExMem{
	
	private:
		Alu *alu;
		Id *idStage;
		Controle *sinaisControle;
		
	public:
		ExMem(Id *idStage, Controle *sinaisControle);
		void realizarOperacoes();
};

ExMem::ExMem(Id *idStage, Controle *sinaisControle) {

	this->idStage = idStage;
	this->sinaisControle = sinaisControle;
	alu = new Alu(idStage, sinaisControle);
}

void ExMem::realizarOperacoes(){
	
	// Realiza os calculos aritmeticos
	if((sinaisControle->getBranch() == 0) && (sinaisControle->getJump() == 0) && (sinaisControle->getMemwrite() == 0) && (sinaisControle->getMemread() == 0))
		alu->instrucoesAritmeticas();
		
	// Realiza os calculos dos desvios
	else if((sinaisControle->getBranch() == 1) || (sinaisControle->getJump() == 1))
		alu->instrucoesDeDesvio();
		
	// Realiza os calculos para armazenar valores na memoria
	else if((sinaisControle->getBranch() == 0) && (sinaisControle->getJump() == 0) && ((sinaisControle->getMemwrite() == 1) || (sinaisControle->getMemread() == 1)))
		alu->instrucoesDeMemoria();
}
