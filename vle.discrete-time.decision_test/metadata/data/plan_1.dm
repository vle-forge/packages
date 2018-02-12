<?xml version='1.0' encoding='UTF-8'?>
<!DOCTYPE vle_project_metadata>
<vle_project_metadata version="1.x" author="meto">
 <dataPlugin package="vle.discrete-time.decision" name="Plan Activities"/>
 <dataModel outputsType="discrete-time" package="vle.discrete-time.decision_test" conf="plan_1"/>
 <definition>
  <activities>
   <activity timeLag="1" deadline="1" value="-131" y="-101" minstart="2000-1-9" x="-630" name="activity" maxfinish="2000-1-10" maxiter="1">
    <outputParams>
     <outputParam value="1" name="outputParam" type="Value"/>
     
     
    </outputParams>
    <rulesAssigment>
     <rule name="rule"/>
    </rulesAssigment>
   </activity>
  </activities>
  <parameters>
   
  </parameters>
  <predicates>
   <predicate operator="!=" leftValue="0." leftType="Val" rightValue="0." name="predicate" rightType="Val"/>
  </predicates>
  <rules>
   <rule name="rule">
    <predicate name="predicate"/>
   </rule>
  </rules>
  <precedences/>
 </definition>
 <configuration>
  <dynamic library="agentDTG" package="vle.discrete-time.decision" name="dynagentDTG"/>
  <observable name="obsplan_1">
   <port name="KnowledgeBase"/>
   <port name="AchievedPlan"/>
   <port name="Activities"/>
   <port name="Activity_activity"/>
   <port name="Activity(state)_activity"/>
   <port name="Activity(ressources)_activity"/>
   <port name="outputParam"/>
   
   
   
  </observable>
  <condition name="condplan_1">
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
        <string>plan_1</string>
       </set>
      </set>
     </key>
    </map>
   </port>
   <port name="aDTGParameters">
    <map/>
   </port>
  </condition>
  <in>
   
  </in>
  <out>
   <port name="outputParam"/>
   
   
  </out>
 </configuration>
</vle_project_metadata>
