#define LOGIC__nenKALMAN_R_CONST	0.2f//More Important to Predict Value
#define LOGIC__nenKALMAN_Q_CONST    0.01f//More Important to Current Value
#define LOGIC__nenKALMAN_I_CONST	1.0f

typedef struct
{
	float f32PredictXk;
	float f32CovPk;
	float f32PredictXkPrev;
	float f32CovPkPrev;
	float f32KalmanGain;
}LOGIC__tstKalmanFilterCtrl;

LOGIC__tstKalmanFilterCtrl LOGIC__stKalmanFilterCtrl[DEF_NUM_CHANNELS];//3 Channel

void LOGIC__vKalmanInit(float f32SignalVal, LOGIC__tstKalmanFilterCtrl *pstKalmanCtrl)
{
	pstKalmanCtrl->f32PredictXk = f32SignalVal;
	pstKalmanCtrl->f32PredictXkPrev = f32SignalVal;
	pstKalmanCtrl->f32CovPk = 1.0f;
	pstKalmanCtrl->f32CovPkPrev = 1.0f;
}

float LOGIC__fKalmanFilter(float f32SignalVal, LOGIC__tstKalmanFilterCtrl *pstKalmanCtrl)
{
	float  fNumerator = 0.0f;
	float  fDenominator = 0.0f;
	
	//STEP1 Calculation KalmanGain
	fNumerator = pstKalmanCtrl->f32CovPkPrev + LOGIC__nenKALMAN_Q_CONST;
	fDenominator = pstKalmanCtrl->f32CovPkPrev + LOGIC__nenKALMAN_Q_CONST + LOGIC__nenKALMAN_R_CONST;
	
	pstKalmanCtrl->f32KalmanGain = (fNumerator/fDenominator); 
	
	//STEP2 Calculation Estimate Value
	pstKalmanCtrl->f32PredictXk = f32SignalVal - pstKalmanCtrl->f32PredictXkPrev;
	pstKalmanCtrl->f32PredictXk *= pstKalmanCtrl->f32KalmanGain;
	pstKalmanCtrl->f32PredictXk += pstKalmanCtrl->f32PredictXkPrev;
	
	//STEP3 Calculation New Conv Value
	pstKalmanCtrl->f32CovPk = LOGIC__nenKALMAN_I_CONST - pstKalmanCtrl->f32KalmanGain;
	pstKalmanCtrl->f32CovPk *= (pstKalmanCtrl->f32CovPkPrev + LOGIC__nenKALMAN_Q_CONST);
	
	//STEP4 Update Previous Value
	pstKalmanCtrl->f32PredictXkPrev = pstKalmanCtrl->f32PredictXk;
	pstKalmanCtrl->f32CovPkPrev = pstKalmanCtrl->f32CovPk;
	
	return pstKalmanCtrl->f32PredictXk;
}