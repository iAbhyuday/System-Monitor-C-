
/*******************************************************************************
* Title                 :   Processor Class
* Filename              :   processor.cpp
* Author                :   Fernando Kaba Surjus
* Origin Date           :   20/05/2020
* Version               :   1.0.0
* Compiler              :   G++17
* Target                :   LINUX
* Notes                 :   None
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESSED
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL AUTHOR OR ITS CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************/
/*************** SOURCE REVISION LOG *****************************************
*
*    Date    Version   Author         Description 
*  20/05/20   1.0.0   Fernandoks   Initial Release.
*
*******************************************************************************/
/** @file : processor.cpp
 *  @brief This is the source file for processor Class, this class is 
 *  responsible the read the /proc files from Linux, and process the
 *  information.
 *  This Class use the linux_parser.h as file/string parser.
 * 
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "processor.h"
#include "linux_parser.h"


/******************************************************************************
* Function Definitions
*******************************************************************************/


/******************************************************************************
* Method : Processor::Utilization()
*//** 
* \b Description:
*  This functions calculates the actual CPU utilization, based on the vector
*  received from the "linux_parser" function
*  std::vector<long> cpuData = LinuxParser::ProcData("cpu");
* PRE-CONDITION: This method needs a Processor Class instance, and a valid 
*                cpu name
* POST-CONDITION: If any error occurs will return 0.
* @return 		float value.
* @see Processor::Utilization
*
* <br><b> - HISTORY OF CHANGES - </b>
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 20/05/2020 </td><td> 1.0.0            </td><td> FKS      </td><td> Method tested </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
float Processor::Utilization() {
  std::vector<long> cpuData = LinuxParser::ProcData("cpu");
  actualCPUdata.user = cpuData[0];
  actualCPUdata.nice = cpuData[1];
  actualCPUdata.system = cpuData[2];
  actualCPUdata.idle = cpuData[3];
  actualCPUdata.iowait = cpuData[4];
  actualCPUdata.irq = cpuData[5];
  actualCPUdata.softirq = cpuData[6];
  actualCPUdata.steal = cpuData[7];
  actualCPUdata.guest = cpuData[8];
  actualCPUdata.guest_nice = cpuData[9];
  cpuData.clear();

  long PrevIdle = prevCPUdata.idle + prevCPUdata.iowait;
  long Idle = actualCPUdata.idle + actualCPUdata.iowait;

  long PrevNonIdle = prevCPUdata.user + prevCPUdata.nice + prevCPUdata.system +
                     prevCPUdata.irq + prevCPUdata.softirq + prevCPUdata.steal;
  long NonIdle = actualCPUdata.user + actualCPUdata.nice +
                 actualCPUdata.system + actualCPUdata.irq +
                 actualCPUdata.softirq + actualCPUdata.steal;

  long PrevTotal = PrevIdle + PrevNonIdle;
  long Total = Idle + NonIdle;

  // differentiate: actual value minus the previous one
  long totald = Total - PrevTotal;
  long idled = Idle - PrevIdle;

  float CPU_Percentage = (float(totald - idled) / float(totald));

  prevCPUdata = actualCPUdata;

  return CPU_Percentage;
}

/******************************************************************************
* Method : Processor::getMemoryUtilization()
*//** 
* \b Description:
*  This functions converts the LinuxParser::MemoryUtilization()
* PRE-CONDITION: This method needs a valid Processor instance
* POST-CONDITION: None
* @return 		string
* @see Processor::getMemoryUtilization
*
* <br><b> - HISTORY OF CHANGES - </b>
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 20/05/2020 </td><td> 1.0.0            </td><td> FKS      </td><td> Method tested </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
std::string Processor::getMemoryUtilization() {
  return (std::to_string(LinuxParser::MemoryUtilization()));
}


/******************************************************************************
* Method : Processor::getProcDataAll()
*//** 
* \b Description:
*  This functions retrives the /proc data from Linux, using 
*  LinuxParser::ProcData(kCpuName), the information is written in the
   std::map<std::string, std::vector<long>> cpus;
* PRE-CONDITION: This method needs a valid cpu name
* POST-CONDITION: If any error occurs will return -1.
* @return 		Void
* @see Processor::getProcDataAll
*
* <br><b> - HISTORY OF CHANGES - </b>
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 20/05/2020 </td><td> 1.0.0            </td><td> FKS      </td><td> Method tested </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/

void Processor::getProcDataAll() {
  const std::string kCpuName{"cpu"};
  std::map<std::string, std::vector<long>> cpus;
  std::string nCpu;
  int i = 0;

  // Get cpu data
  cpus.emplace(kCpuName, LinuxParser::ProcData(kCpuName));

  // Get other cpu info
  do {
    nCpu = kCpuName + std::to_string(i++);
    cpus.emplace(nCpu, LinuxParser::ProcData(nCpu));
  } while (cpus[nCpu].back() != -1);
}
