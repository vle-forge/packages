
#'
#' transform a date in julian day
#' 
#' @export
#' 
#' @param datStr, eg :  "2014-01-01"
#'   
#' @return the date in julian day: 2456659
#' 
#' @author Ronan Trépos MIA-T, INRA
#' 
#' @note

vle.recursive.dateToNum = function (dateStr)
{
  return (as.numeric(as.Date(dateStr, format="%Y-%m-%d") + 2440588));
}


#'
#' transform a date of the form 2456659 into an object Date
#' 
#' @export
#' 
#' @param dateNum, eg :  2456659
#'   
#' @return return a date object, eg:  as.Date("2014-01-01")
#' 
#' @author Ronan Trépos MIA-T, INRA
#' 
#' @note

vle.recursive.dateFromNum = function(dateNum)
{
  return(as.Date(as.Date(dateNum, origin="1970-01-01") - 2440588));
}


#'
#' init the embedded simulator
#' 
#' @export
#' 
#' @param pkg, package where is located the embedded simulator
#' @param file, vpz file that identifies the embedded simulator
#'   
#' @return a rvle object of vle recursive model
#' 
#' @author Ronan Trépos MIA-T, INRA
#' 
#' @note
#' 
#' Will build the rvle_obj (a Rvle object)
#' 
#' @examples
#' 
#' #TODO
#'  
vle.recursive.init = function(pkg=NULL, file=NULL)
{
    rvle_obj = new("Rvle", pkg="vle.recursive", file="vle-recursive.vpz");
    rvle.addCondition(rvle_obj@sim, "cond");
    rvle.addPort(rvle_obj@sim, "cond", "vpz");
    rvle.addPort(rvle_obj@sim, "cond", "package");
    rvle.setStringCondition(rvle_obj@sim, "cond", "package", pkg);
    rvle.setStringCondition(rvle_obj@sim, "cond", "vpz", file);
    return(rvle_obj);
}

#'
#' show the content of the embedded simulator
#' 
#' @export
#'
#' @param rvle_obj, a rvle object of vle recursive model
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
vle.recursive.showEmbedded = function(rvle_obj=NULL)
{
  ftmp = vle.recursive.getEmbedded(rvle_obj);
  show(ftmp);
  rm(ftmp);
}

#'
#' get a Rvle Object of the ebmedded model
#' 
#' @export
#'
#' @param rvle_obj, a rvle object of vle recursive model
#' @param replicate, index of the replicate for embedded
#'                   model configuration
#' 
#' @return a Rvle object of the embedded simulator
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
vle.recursive.getEmbedded = function(rvle_obj=NULL, replicate=1)
{
  ftmp = new("Rvle", pkg=getDefault(rvle_obj, "cond.package"), 
             file=getDefault(rvle_obj, "cond.vpz"));
  for (cond in rvle.listConditions(rvle_obj@sim)) {
    for (port in rvle.listConditionPorts(rvle_obj@sim, cond)) {
      if (startsWith(port, "propagate_")) {
        econdPort = strsplit(port, split="propagate_")[[1]][2]
        econd = strsplit(econdPort, split="\\.")[[1]][1]
        eport = strsplit(econdPort, split="\\.")[[1]][2]
        rvle.setValueCondition(ftmp@sim, econd, eport,
           rvle.getConditionPortValues(rvle_obj@sim, 
              cond, port));                  
      } else if (startsWith(port, "input_")) {	
        #TODO should get inputs

      } else if (startsWith(port, "replicate_")) {
	#TODO should get replicate id 'replicate'
      }      
    }  
  }
  return(ftmp);
}

#'
#' init one input
#' 
#' @export
#'
#' @param rvle_obj, a rvle object of vle recursive model
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

vle.recursive.configPropagate = function(rvle_obj=NULL, propagate=NULL, value=NULL)
{
  if (is.null(rvle_obj)) {
    print("[vle.recursive] error missing rvle_obj");
    return (NULL);
  }
  inputPort = paste("propagate", sep="_", propagate);
  
  listPorts = rvle.listConditionPorts(rvle_obj@sim, "cond");
  if (sum(listPorts == inputPort) ==0) {
    rvle.addPort(rvle_obj@sim, "cond", inputPort);
  }
  #avoid MULTIPLE values
  classVal = class(value);
  if (is.null(classVal) || !grepl("^Vle", classVal)) {
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
  rvle.setValueCondition(rvle_obj@sim, cond="cond", port=inputPort, 
                         value);
  
  #remove replicate and input with same name
  inputPort = paste("replicate", sep="_", propagate);
  listPorts = rvle.listConditionPorts(rvle_obj@sim, "cond");
  if (sum(listPorts == inputPort) != 0) {
    rvle.removePort(rvle_obj@sim, "cond", inputPort);
  }
  inputPort = paste("input", sep="_", propagate);
  listPorts = rvle.listConditionPorts(rvle_obj@sim, "cond");
  if (sum(listPorts == inputPort) != 0) {
    rvle.removePort(rvle_obj@sim, "cond", inputPort);
  }
}


#'
#' init one input
#' 
#' @export
#' 
#' @param rvle_obj, a rvle object of vle recursive model
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
    
vle.recursive.configInput = function(rvle_obj=NULL, input=NULL, values=NULL)
{
  if (is.null(rvle_obj)) {
    print("[vle.recursive] error missing rvle_obj");
    return (NULL);
  }
  inputPort = paste("input", sep="_", input);
  
  listPorts = rvle.listConditionPorts(rvle_obj@sim, "cond");
  if (sum(listPorts == inputPort) ==0) {
    rvle.addPort(rvle_obj@sim, "cond", inputPort);
  }
  #avoid MULTIPLE values
  classVal = class(values);
  if (is.null(classVal) || !grepl("^Vle", classVal)) {
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
  rvle.setValueCondition(rvle_obj@sim, cond="cond", port=inputPort, 
            values);

  #remove replicate and propagate with same name
  inputPort = paste("replicate", sep="_", input);
  listPorts = rvle.listConditionPorts(rvle_obj@sim, "cond");
  if (sum(listPorts == inputPort) != 0) {
    rvle.removePort(rvle_obj@sim, "cond", inputPort);
  }
  inputPort = paste("propagate", sep="_", input);
  listPorts = rvle.listConditionPorts(rvle_obj@sim, "cond");
  if (sum(listPorts == inputPort) != 0) {
    rvle.removePort(rvle_obj@sim, "cond", inputPort);
  }
}

#'
#' init the replicate condition port
#' 
#' @export
#' 
#' @param rvle_obj, a rvle object of vle recursive model
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

vle.recursive.configReplicate = function(rvle_obj=NULL, replicate=NULL, values=NULL)
{
  if (is.null(rvle_obj)) {
    print("[vle.recursive] error missing rvle_obj");
    return (NULL);
  }
  inputPort = paste("replicate", sep="_", replicate);
  listPorts = rvle.listConditionPorts(rvle_obj@sim, "cond");
  if (sum(listPorts == inputPort) ==0) {
    rvle.addPort(rvle_obj@sim, "cond", inputPort);
  }
  #avoid MULTIPLE values
  classVal = class(values);
  if (is.null(classVal) || !grepl("^Vle", classVal)) {
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
  rvle.setValueCondition(rvle_obj@sim, cond="cond", port=inputPort, 
                         values);
  
  #remove input and propagate with same name
  inputPort = paste("input", sep="_", replicate);
  listPorts = rvle.listConditionPorts(rvle_obj@sim, "cond");
  if (sum(listPorts == inputPort) != 0) {
    rvle.removePort(rvle_obj@sim, "cond", inputPort);
  }
  inputPort = paste("propagate", sep="_", replicate);
  listPorts = rvle.listConditionPorts(rvle_obj@sim, "cond");
  if (sum(listPorts == inputPort) != 0) {
    rvle.removePort(rvle_obj@sim, "cond", inputPort);
  }
}

#'
#' Get value of either an input a replicate a propagate or an embedded value
#' 
#' @export
#' 
#' @param rvle_obj, a rvle object of vle recursive model
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
vle.recursive.getValue = function(rvle_obj=NULL, id=NULL)
{
  inputPort = paste("input", sep="_", id);
  ##try to find into rvle_obj an input
  if (length(which(rvle.listConditionPorts(rvle_obj@sim,"cond") == 
                   inputPort)) > 0) {
    return(rvle.getConditionPortValues(rvle_obj@sim, 
                                       "cond", inputPort));
  }
  
  inputPort = paste("replicate", sep="_", id);
  ##try to find into rvle_obj a replicate
  if (length(which(rvle.listConditionPorts(rvle_obj@sim,"cond") == 
                   inputPort)) > 0) {
    return(rvle.getConditionPortValues(rvle_obj@sim, 
                                       "cond", inputPort));
  }
  
  inputPort = paste("propagate", sep="_", id);
  ##try to find into rvle_obj a replicate
  if (length(which(rvle.listConditionPorts(rvle_obj@sim,"cond") == 
                   inputPort)) > 0) {
    return(rvle.getConditionPortValues(rvle_obj@sim, 
                                       "cond", inputPort));
  }

  ##try to find into embedded sim
  ftmp = new("Rvle", pkg=getDefault(rvle_obj, "cond.package"), 
            file=getDefault(rvle_obj, "cond.vpz"));
  return (getDefault(ftmp, id));
}

#'
#' config one output
#' 
#' @export
#' 
#' @param rvle_obj, a rvle object of vle recursive model
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
vle.recursive.configOutput = function(rvle_obj=NULL, id=NULL, path=NULL, 
        mse_observations=NULL, mse_times=NULL, integration="all", 
        aggregation_input="all")
{
    if (is.null(rvle_obj)) {
        print("[vle.recursive] error missing rvle_obj");
        return (NULL);
    }
    outputPort = paste("output", sep="_", id);
    listPorts = rvle.listConditionPorts(rvle_obj@sim, "cond");
    if (sum(listPorts == outputPort) == 0) {
        rvle.addPort(rvle_obj@sim, "cond", outputPort);
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
    rvle.setValueCondition(rvle_obj@sim, cond="cond", port=outputPort, 
            config);
#    rvle.addObservablePort(rvle_obj@sim, "obs", id)
#    rvle.attachView(rvle_obj@sim, "view", "obs", id)
}

#'
#' config the type of simulations
#' 
#' @export
#' 
#' @param rvle_obj, a rvle object of vle recursive model
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

vle.recursive.configSimulation = function(rvle_obj=NULL, config_parallel_type=NULL, 
        config_parallel_rm_files=NULL, config_parallel_nb_slots=NULL, 
        working_dir=NULL)
{
    if (is.null(rvle_obj)) {
        print("[vle.recursive] error missing rvle_obj");
        return (NULL);
    }
    listPorts = rvle.listConditionPorts(rvle_obj@sim, "cond");
    if (sum(listPorts == "config_parallel_type") == 0) {
        rvle.addPort(rvle_obj@sim, "cond", "config_parallel_type");
    }
    if (sum(listPorts == "config_parallel_rm_files") == 0) {
        rvle.addPort(rvle_obj@sim, "cond", "config_parallel_rm_files");
    }
    if (sum(listPorts == "config_parallel_nb_slots") == 0) {
        rvle.addPort(rvle_obj@sim, "cond", "config_parallel_nb_slots");
    }
    if (sum(listPorts == "working_dir") == 0) {
        rvle.addPort(rvle_obj@sim, "cond", "working_dir");
    }
    if (is.null(config_parallel_type)) {
        rvle.setValueCondition(rvle_obj@sim, cond="cond",
                port="config_parallel_type", "single");
    } else {
        rvle.setValueCondition(rvle_obj@sim, cond="cond",
                port="config_parallel_type", config_parallel_type);
    }
    if (is.null(config_parallel_rm_files)) {
        rvle.setValueCondition(rvle_obj@sim, cond="cond",
                port="config_parallel_rm_files", FALSE);
    } else {
        rvle.setValueCondition(rvle_obj@sim, cond="cond",
                port="config_parallel_rm_files", config_parallel_rm_files);
    }
    if (is.null(config_parallel_nb_slots)) {
        rvle.setValueCondition(rvle_obj@sim, cond="cond",
                port="config_parallel_nb_slots", as.integer(1));
    } else {
        rvle.setValueCondition(rvle_obj@sim, cond="cond",
                port="config_parallel_nb_slots",
                as.integer(config_parallel_nb_slots));
    }
    if (is.null(working_dir)) {
        rvle.setValueCondition(rvle_obj@sim, cond="cond",
                port="working_dir", "/tmp/");
    } else {
        rvle.setValueCondition(rvle_obj@sim, cond="cond",
                port="working_dir", working_dir);
    }
}


#'
#' simulates the experiment plan
#' 
#' @export
#' 
#' @param rvle_obj, a rvle object of vle recursive model
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

vle.recursive.simulate = function(rvle_obj=NULL)
{
  if (is.null(rvle_obj)) {
    print("[vle.recursive] error missing rvle_obj");
    return (NULL);
  }
  res = rvle.run(rvle_obj@sim)$view$"vle-recursive:vle_recursive.outputs"[[1]];
  if (is.null(res)) {
    savedVpz = paste(Sys.getenv("VLE_HOME"),sep="/",
                     "pkgs-2.0/vle.recursive/exp/test_error.vpz"); 
    saveVpz(rvle_obj, savedVpz);
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
