

rvle_recursive = NULL;

#'
#' init the embedded simulator
#' 
#' @export
#' 
#' @param pkg, package where is located the embedded simulator
#' @param file, vpz file that identifies the embedded simulator
#'   
#' @return NULL
#' 
#' @author Ronan Trépos MIA-T, INRA
#' 
#' @note
#' 
#' Will build the rvle_recursive (a Rvle object)
#' 
#' @examples
#' 
#' #TODO
#'  
vle.recursive.init = function(pkg, file)
{
    if (! is.null(rvle_recursive)) {
        rvle_recursive = NULL;
    }
    rvle_recursive = new("Rvle", pkg="vle.recursive", file="vle-recursive.vpz");
    rvle.addCondition(rvle_recursive@sim, "cond");
    rvle.addPort(rvle_recursive@sim, "cond", "vpz");
    rvle.addPort(rvle_recursive@sim, "cond", "package");
    rvle.setStringCondition(rvle_recursive@sim, "cond", "package", pkg);
    rvle.setStringCondition(rvle_recursive@sim, "cond", "vpz", file);
#    rvle.removeObservablePort(rvle_recursive@sim, "obs", "outputs")
    assign("rvle_recursive", rvle_recursive, env=.GlobalEnv)
}


#'
#' init one input
#' 
#' @export
#' 
#' @param propagate, a string of the form 'cond.port'
#' @param value, the value to propagate to the embedded simulator
#' 
#' @return NULL
#' 
#' @author Ronan Trépos MIA-T, INRA
#' 
#' @note
#' 
#' Nothing
#' 
#' @examples
#' 
#' #TODO
#'  

vle.recursive.configPropagate = function(propagate=NULL, value=NULL)
{
  if (is.null(rvle_recursive)) {
    print("[vle.recursive] error missing initialization");
    return (NULL);
  }
  inputPort = paste("propagate", sep="_", propagate);
  
  listPorts = rvle.listConditionPorts(rvle_recursive@sim, "cond");
  if (sum(listPorts == inputPort) ==0) {
    rvle.addPort(rvle_recursive@sim, "cond", inputPort);
  }
  #avoid MULTIPLE values
  classVal = class(value);
  if (is.null(classVal) || !startsWith(classVal, "Vle")) {
    if (is.numeric(value) && (length(value)>1)) {
      class(value) <- "VleTUPLE";
    } else if (!is.null(length(value)) &&  (length(value)>1)) {
      class(value) <- "VleSET";
    }  
  } else {
    if (classVal == "VleMULTIPLE_VALUES") {
      stop("[R vle.recursise] configuring a propagate with multiple values");
    }
  }
  rvle.setValueCondition(rvle_recursive@sim, cond="cond", port=inputPort, 
                         value);
  
  #remove replicate and input with same name
  inputPort = paste("replicate", sep="_", propagate);
  listPorts = rvle.listConditionPorts(rvle_recursive@sim, "cond");
  if (sum(listPorts == inputPort) != 0) {
    rvle.removePort(rvle_recursive@sim, "cond", inputPort);
  }
  inputPort = paste("input", sep="_", propagate);
  listPorts = rvle.listConditionPorts(rvle_recursive@sim, "cond");
  if (sum(listPorts == inputPort) != 0) {
    rvle.removePort(rvle_recursive@sim, "cond", inputPort);
  }
}


#'
#' init one input
#' 
#' @export
#' 
#' @param input, a string of the form 'cond.port'
#' @param values, the set of values to simulate
#' 
#' @return NULL
#' 
#' @author Ronan Trépos MIA-T, INRA
#' 
#' @note
#' 
#' Nothing
#' 
#' @examples
#' 
#' #TODO
#'  
    
vle.recursive.configInput = function(input=NULL, values=NULL)
{
  if (is.null(rvle_recursive)) {
    print("[vle.recursive] error missing initialization");
    return (NULL);
  }
  inputPort = paste("input", sep="_", input);
  
  listPorts = rvle.listConditionPorts(rvle_recursive@sim, "cond");
  if (sum(listPorts == inputPort) ==0) {
    rvle.addPort(rvle_recursive@sim, "cond", inputPort);
  }
  #avoid MULTIPLE values
  classVal = class(values);
  if (is.null(classVal) || !startsWith(classVal, "Vle")) {
    if (is.numeric(values) && (length(values)>1)) {
      class(values) <- "VleTUPLE";
    } else if (!is.null(length(values)) &&  (length(values)>1)) {
      class(values) <- "VleSET";
    }  
  } else {
    if (classVal == "VleMULTIPLE_VALUES") {
      stop("[R vle.recursise] configuring an input with multiple values");
    }
  }
  rvle.setValueCondition(rvle_recursive@sim, cond="cond", port=inputPort, 
            values);

  #remove replicate and propagate with same name
  inputPort = paste("replicate", sep="_", input);
  listPorts = rvle.listConditionPorts(rvle_recursive@sim, "cond");
  if (sum(listPorts == inputPort) != 0) {
    rvle.removePort(rvle_recursive@sim, "cond", inputPort);
  }
  inputPort = paste("propagate", sep="_", input);
  listPorts = rvle.listConditionPorts(rvle_recursive@sim, "cond");
  if (sum(listPorts == inputPort) != 0) {
    rvle.removePort(rvle_recursive@sim, "cond", inputPort);
  }
}

#'
#' init the replicate condition port
#' 
#' @export
#' 
#' @param replicate, a string of the form 'cond.port'
#' @param values, the set of values to simulate
#' 
#' @return NULL
#' 
#' @author Ronan Trépos MIA-T, INRA
#' 
#' @note
#' 
#' Nothing
#' 
#' @examples
#' 
#' #TODO
#'  

vle.recursive.configReplicate = function(replicate=NULL, values=NULL)
{
  if (is.null(rvle_recursive)) {
    print("[vle.recursive] error missing initialization");
    return (NULL);
  }
  inputPort = paste("replicate", sep="_", replicate);
  listPorts = rvle.listConditionPorts(rvle_recursive@sim, "cond");
  if (sum(listPorts == inputPort) ==0) {
    rvle.addPort(rvle_recursive@sim, "cond", inputPort);
  }
  #avoid MULTIPLE values
  classVal = class(values);
  if (is.null(classVal) || !startsWith(classVal, "Vle")) {
    if (is.numeric(values) && (length(values)>1)) {
      class(values) <- "VleTUPLE";
    } else if (!is.null(length(values)) &&  (length(values)>1)) {
      class(values) <- "VleSET";
    }  
  } else {
    if (classVal == "VleMULTIPLE_VALUES") {
      stop("[R vle.recursise] configuring a replicate with multiple values");
    }
  }
  rvle.setValueCondition(rvle_recursive@sim, cond="cond", port=inputPort, 
                         values);
  
  #remove input and propagate with same name
  inputPort = paste("input", sep="_", replicate);
  listPorts = rvle.listConditionPorts(rvle_recursive@sim, "cond");
  if (sum(listPorts == inputPort) != 0) {
    rvle.removePort(rvle_recursive@sim, "cond", inputPort);
  }
  inputPort = paste("propagate", sep="_", replicate);
  listPorts = rvle.listConditionPorts(rvle_recursive@sim, "cond");
  if (sum(listPorts == inputPort) != 0) {
    rvle.removePort(rvle_recursive@sim, "cond", inputPort);
  }
}

#'
#' Get value of either an input a replicate a propagate or an embedded value
#' 
#' @export
#' 
#' @param id, a string of the form 'cond.port'
#' 
#' @return NULL
#' 
#' @author Ronan Trépos MIA-T, INRA
#' 
#' @note
#' 
#' Nothing
#' 
#' @examples
#' 
#' #TODO
#'  
vle.recursive.getValue = function(id=NULL)
{
  inputPort = paste("input", sep="_", id);
  ##try to find into rvle_recursive an input
  if (length(which(rvle.listConditionPorts(rvle_recursive@sim,"cond") == 
                   inputPort)) > 0) {
    return(rvle.getConditionPortValues(rvle_recursive@sim, 
                                       "cond", inputPort));
  }
  
  inputPort = paste("replicate", sep="_", id);
  ##try to find into rvle_recursive a replicate
  if (length(which(rvle.listConditionPorts(rvle_recursive@sim,"cond") == 
                   inputPort)) > 0) {
    return(rvle.getConditionPortValues(rvle_recursive@sim, 
                                       "cond", inputPort));
  }
  
  inputPort = paste("propagate", sep="_", id);
  ##try to find into rvle_recursive a replicate
  if (length(which(rvle.listConditionPorts(rvle_recursive@sim,"cond") == 
                   inputPort)) > 0) {
    return(rvle.getConditionPortValues(rvle_recursive@sim, 
                                       "cond", inputPort));
  }

  ##try to find into embedded sim
  ftmp = new("Rvle", pkg=getDefault(rvle_recursive, "cond.package"), 
            file=getDefault(rvle_recursive, "cond.vpz"));
  return (getDefault(ftmp, id));
}

#'
#' config one output
#' 
#' @export
#' 
#' @param id, the id (a string) of the output
#' @param path, a string of the form  'viewname/pathOfTheAtomicModel.ObsPort'
#' that identifies the column to get from the simulation of the embedded
#' simulator
#' @param mse_observations, list of observations (required only if 
#' integration="mse")
#' @param mse_times, times of observations
#' @param integration, amongst "last", "max", "mse" or "all"
#' @param aggregation_input, amongst "mean", "quantile", "max" or "all"
#' 
#' @return NULL
#' 
#' @author Ronan Trépos MIA-T, INRA
#' 
#' @note
#' 
#' Nothing
#' 
#' @examples
#' 
#' #TODO
#' 
vle.recursive.configOutput = function(id=NULL, path=NULL, 
        mse_observations=NULL, mse_times=NULL, integration="all", 
        aggregation_input="all")
{
    if (is.null(rvle_recursive)) {
        print("[vle.recursive] error missing initialization");
        return (NULL);
    }
    outputPort = paste("output", sep="_", id);
    listPorts = rvle.listConditionPorts(rvle_recursive@sim, "cond");
    if (sum(listPorts == outputPort) == 0) {
        rvle.addPort(rvle_recursive@sim, "cond", outputPort);
    }
    
    config = NULL;
    if (integration == "mse") {
        
        config = list(id=id, path=path, integration=integration, 
                aggregation_input=aggregation_input, mse_times = mse_times,
                mse_observations = mse_observations);
    } else {
        
        config = list(id=id, path=path, integration=integration, 
                aggregation_input=aggregation_input);   
    }
    class(config) <- "VleMAP";
    rvle.setValueCondition(rvle_recursive@sim, cond="cond", port=outputPort, 
            config);
#    rvle.addObservablePort(rvle_recursive@sim, "obs", id)
#    rvle.attachView(rvle_recursive@sim, "view", "obs", id)
}

#'
#' config the type of simulations
#' 
#' @export
#' 
#' @param config_parallel_type (string amongst threads, mvle and single). 
#'  It sets the type of parallelization to perform.
#' @param config_parallel_rm_files (bool; default true). 
#'  Used only if config_parallel_type is set to mvle. Simulation files created
#'  into directory working_dir are removed after analysis.
#' @param config_parallel_nb_slots (int > 0). it gives the number of slots to 
#'  use for parallelization.
#' @param working_dir.  Required only if config_parallel_type is set to mvle. 
#' It gives the working directory where are produced result file of single 
#' simulations.
#' 
#' @return NULL
#' 
#' @author Ronan Trépos MIA-T, INRA
#' 
#' @note
#' 
#' Nothing
#' 
#' @examples
#' 
#' #TODO
#' 

vle.recursive.configSimulation = function(config_parallel_type=NULL, 
        config_parallel_rm_files=NULL, config_parallel_nb_slots=NULL, 
        working_dir=NULL)
{
    if (is.null(rvle_recursive)) {
        print("[vle.recursive] error missing initialization");
        return (NULL);
    }
    listPorts = rvle.listConditionPorts(rvle_recursive@sim, "cond");
    if (sum(listPorts == "config_parallel_type") == 0) {
        rvle.addPort(rvle_recursive@sim, "cond", "config_parallel_type");
    }
    if (sum(listPorts == "config_parallel_rm_files") == 0) {
        rvle.addPort(rvle_recursive@sim, "cond", "config_parallel_rm_files");
    }
    if (sum(listPorts == "config_parallel_nb_slots") == 0) {
        rvle.addPort(rvle_recursive@sim, "cond", "config_parallel_nb_slots");
    }
    if (sum(listPorts == "working_dir") == 0) {
        rvle.addPort(rvle_recursive@sim, "cond", "working_dir");
    }
    if (is.null(config_parallel_type)) {
        rvle.setValueCondition(rvle_recursive@sim, cond="cond",
                port="config_parallel_type", "single");
    } else {
        rvle.setValueCondition(rvle_recursive@sim, cond="cond",
                port="config_parallel_type", config_parallel_type);
    }
    if (is.null(config_parallel_rm_files)) {
        rvle.setValueCondition(rvle_recursive@sim, cond="cond",
                port="config_parallel_rm_files", FALSE);
    } else {
        rvle.setValueCondition(rvle_recursive@sim, cond="cond",
                port="config_parallel_rm_files", config_parallel_rm_files);
    }
    if (is.null(config_parallel_nb_slots)) {
        rvle.setValueCondition(rvle_recursive@sim, cond="cond",
                port="config_parallel_nb_slots", as.integer(1));
    } else {
        rvle.setValueCondition(rvle_recursive@sim, cond="cond",
                port="config_parallel_nb_slots",
                as.integer(config_parallel_nb_slots));
    }
    if (is.null(working_dir)) {
        rvle.setValueCondition(rvle_recursive@sim, cond="cond",
                port="working_dir", "/tmp/");
    } else {
        rvle.setValueCondition(rvle_recursive@sim, cond="cond",
                port="working_dir", working_dir);
    }
}


#'
#' simulates the experiment plan
#' 
#' @export
#' 
#' @param 
#' 
#' @return results structure
#' 
#' @author Ronan Trépos MIA-T, INRA
#' 
#' @note
#' 
#' Nothing
#' 
#' @examples
#' 
#' #TODO
#' 

vle.recursive.simulate = function()
{
  if (is.null(rvle_recursive)) {
    print("[vle.recursive] error missing initialization");
    return (NULL);
  }
  res = rvle.run(rvle_recursive@sim)$view$"vle-recursive:vle_recursive.outputs"[[1]];
  if (is.null(res)) {
    savedVpz = paste(Sys.getenv("VLE_HOME"),sep="/",
                     "pkgs-2.0/vle.recursive/exp/test_error.vpz"); 
    saveVpz(rvle_recursive, savedVpz);
    rvlelog = paste(Sys.getenv("VLE_HOME"),sep="/", "rvle.log");
    print(paste("[vle.recursive] error in simul, file log date: ", 
                file.info(rvlelog)$mtime));
    if (file.exists(rvlelog)) {
      cat(readLines(rvlelog),sep="\n")
    }
    stop(paste("see saved vpz:", savedVpz));
  }
  class(res) = "vle.recursive.simulate";
  return (res)
}