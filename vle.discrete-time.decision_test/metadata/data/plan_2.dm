<?xml version='1.0' encoding='UTF-8'?>
<!DOCTYPE vle_project_metadata>
<vle_project_metadata version="1.x" author="meto">
 <dataPlugin package="vle.discrete-time.decision" name="Plan Activities"/>
 <dataModel outputsType="discrete-time" package="vle.discrete-time.decision_test" conf="plan_2"/>
 <definition>
  <activities>
   <activity timeLag="1" deadline="0" value="-131" y="-145" minstart="2000-1-10" x="-486" name="activity" maxfinish="" maxiter="1">
    <outputParams>
     <outputParam value="1" name="outputParam" type="Value"/>
     
     
    </outputParams>
    <rulesAssigment>
     <rule name="ruleEqual"/>
    </rulesAssigment>
   </activity>
  <activity timeLag="1" deadline="0" value="-711" y="-37" minstart="2000-1-10" x="-486" name="activity_1" maxfinish="" maxiter="1">
    <outputParams>
     <outputParam value="1" name="outputParam_1" type="Value"/>
    </outputParams>
    <rulesAssigment>
     <rule name="ruleNotEqu_2"/>
     <rule name="ruleNotEqu_1"/>
     <rule name="ruleNotEqu_0"/>
    </rulesAssigment>
   </activity>
  </activities>
  <parameters>
   
  <parameter value="0" name="ParameterName" type="Par"/>
   <parameter value="01-01" name="DayOfYearParameterName" type="dayOfYearPar"/>
   <parameter value="2000-01-01" name="DayParameterName" type="dayPar"/>
  </parameters>
  <predicates>
   <predicate operator="==" leftValue="0." leftType="Val" rightValue="0." name="predicate" rightType="Val"/>
  <predicate operator="==" leftValue="variableName" leftType="Var" rightValue="variableName" name="predicate_1" rightType="Var"/>
   <predicate operator="==" leftValue="ParameterName" leftType="Par" rightValue="ParameterName" name="predicate_2" rightType="Par"/>
   <predicate operator="==" leftValue="day" leftType="dayVar" rightValue="day" name="predicate_3" rightType="dayVar"/>
   <predicate operator="==" leftValue="01-01" leftType="dayVal" rightValue="01-01" name="predicate_4" rightType="dayVal"/>
   <predicate operator="==" leftValue="2000-01-01" leftType="dayOfYearVal" rightValue="2000-01-01" name="predicate_5" rightType="dayOfYearVal"/>
   <predicate operator="==" leftValue="DayOfYearParameterName" leftType="dayOfYearPar" rightValue="DayOfYearParameterName" name="predicate_6" rightType="dayOfYearPar"/>
   <predicate operator="==" leftValue="DayParameterName" leftType="dayPar" rightValue="DayParameterName" name="predicate_7" rightType="dayPar"/>
   <predicate operator="==" leftValue="dayOfYear" leftType="dayOfYearVar" rightValue="dayOfYear" name="predicate_8" rightType="dayOfYearVar"/>
  <predicate operator="==" leftValue="0." leftType="Val" rightValue="1" name="predicate_9" rightType="Val"/>
   <predicate operator="==" leftValue="2000-1-2" leftType="dayVal" rightValue="01-01" name="predicate_10" rightType="dayVal"/>
   <predicate operator="==" leftValue="2000-01-01" leftType="dayOfYearVal" rightValue="1-2" name="predicate_11" rightType="dayOfYearVal"/>
  </predicates>
  <rules>
   <rule name="ruleEqual">
    <predicate name="predicate"/>
   <predicate name="predicate_1"/>
    <predicate name="predicate_2"/>
    <predicate name="predicate_3"/>
    <predicate name="predicate_4"/>
    <predicate name="predicate_5"/>
    <predicate name="predicate_6"/>
    <predicate name="predicate_7"/>
    <predicate name="predicate_8"/>
   </rule>
  <rule name="ruleNotEqu_0">
    <predicate name="predicate_9"/>
   </rule>
   <rule name="ruleNotEqu_2">
    <predicate name="predicate_10"/>
   </rule>
   <rule name="ruleNotEqu_1">
    <predicate name="predicate_11"/>
   </rule>
  </rules>
  <precedences/>
 </definition>
 <configuration>
  <dynamic library="agentDTG" package="vle.discrete-time.decision" name="dynagentDTG"/>
  <observable name="obsplan_2">
   <port name="KnowledgeBase"/>
   <port name="AchievedPlan"/>
   <port name="Activities"/>
   <port name="Activity_activity"/>
   <port name="Activity(state)_activity"/>
   <port name="Activity(ressources)_activity"/>
   <port name="outputParam"/>
   
   
   
  <port name="variableName"/>
  <port name="Activity_activity_1"/>
   <port name="Activity(state)_activity_1"/>
   <port name="Activity(ressources)_activity_1"/>
   <port name="outputParam_1"/>
  </observable>
  <condition name="condplan_2">
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
        <string>plan_2</string>
       </set>
      </set>
     </key>
    </map>
   </port>
   <port name="aDTGParameters">
    <map>
     <key name="DayParameterName">
      <string>2000-01-01</string>
     </key>
     <key name="DayOfYearParameterName">
      <string>01-01</string>
     </key>
     <key name="ParameterName">
      <double>0</double>
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
  </out>
 </configuration>
</vle_project_metadata>
