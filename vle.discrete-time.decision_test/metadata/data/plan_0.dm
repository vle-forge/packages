<?xml version='1.0' encoding='UTF-8'?>
<!DOCTYPE vle_project_metadata>
<vle_project_metadata version="1.x" author="meto">
 <dataPlugin package="vle.discrete-time.decision" name="Plan Activities"/>
 <dataModel outputsType="discrete-time" package="vle.discrete-time.decision_test" conf="plan_0"/>
 <definition>
  <activities>
   <activity timeLag="1" deadline="0" value="-131" y="-151" minstart="2000-1-10" x="-361" name="activity" maxfinish="" maxiter="1">
    <outputParams>
     <outputParam value="1" name="outputParam" type="Value"/>
     <outputParam value="variableName" name="outputParam_1" type="Variable"/>
     <outputParam value="parameterName" name="outputParam_2" type="Parameter"/>
    </outputParams>
    <rulesAssigment/>
   </activity>
  </activities>
  <parameters>
   <parameter value="2" name="parameterName" type="Par"/>
  </parameters>
  <predicates/>
  <rules/>
  <precedences/>
 </definition>
 <configuration>
  <dynamic library="agentDTG" package="vle.discrete-time.decision" name="dynagentDTG"/>
  <observable name="obsplan_0">
   <port name="KnowledgeBase"/>
   <port name="AchievedPlan"/>
   <port name="Activities"/>
   <port name="Activity_activity"/>
   <port name="Activity(state)_activity"/>
   <port name="Activity(ressources)_activity"/>
   <port name="outputParam"/>
   <port name="variableName"/>
   <port name="outputParam_1"/>
   <port name="outputParam_2"/>
  </observable>
  <condition name="condplan_0">
   <port name="dyn_allow">
    <boolean>1</boolean>
   </port>
   <port name="dyn_denys">
    <set/>
   </port>
   <port name="autoAck">
    <boolean>1</boolean>
   </port>
   <port name="PlansLocation">
    <string>vle.discrete-time.decision_test</string>
   </port>
   <port name="Rotation">
    <map>
     <key name="">
      <set>
       <integer>2147483647</integer>
       <set>
        <integer>1</integer>
        <string>plan_0</string>
       </set>
      </set>
     </key>
    </map>
   </port>
   <port name="aDTGParameters">
    <map>
     <key name="parameterName">
      <double>2</double>
     </key>
    </map>
   </port>
  </condition>
  <in>
   <port name="variableName"/>
  </in>
  <out>
   <port name="outputParam"/>
   <port name="outputParam_1"/>
   <port name="outputParam_2"/>
  </out>
 </configuration>
</vle_project_metadata>
