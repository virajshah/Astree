// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef DeviceOptimizer_
#define DeviceOptimizer_

#include <string>
#include <vector>
using namespace std;

#include "OpticalDevice.h"

class DeviceOptimizerParameter
{
public:
    int iSurface;
    string sParameter;
    double dMin;
    double dMax;
    double dVal;
};

enum OptimizerMeritFunction
{
    eCenterOnly,
    eMostlyCenter,
    eFullFrameMean,
    eFullFrameMaxError
};

enum OptimizerResult
{
    eBetterSolutionFound,
    eNoBetterSolution,
    eSolutionOnEdge,
    eNothingToOptimize
};

class DeviceOptimizer
{
public:
    DeviceOptimizer();
    void set_device(OpticalDevice* pDevice);
    void add_parameter(int iSurface,string sParameter,double dMin,double dMax);
    OptimizerResult optimise_random(OptimizerMeritFunction eMeritFunction);

private:
    void apply_parameter(const vector<DeviceOptimizerParameter>& parameters);
    double compute_demerit(OptimizerMeritFunction eMeritFunction); //return demerit value: lower is better

    OpticalDevice* _pDevice;
    vector<DeviceOptimizerParameter> _parameters;
};

#endif
