
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
#' @return a rvle handle of vle recursive model
#' 
#' @author Ronan Trépos MIA-T, INRA
#' 
#' @note
#' 
#' Will build the rvle handle (with rvle.open)
#' 
#' @examples
#' 
#' f = vle.recursive(pkg="mypkg", file="mymodel.vpz")
#'  
vle.recursive.init = function(pkg=NULL, file=NULL)
{
    rvle_handle = rvle.open(pkg="vle.recursive", file="vle-recursive.vpz");
    rvle.addCondition(rvle_handle, "cond");
    rvle.addPort(rvle_handle, "cond", "vpz");
    rvle.addPort(rvle_handle, "cond", "package");
    rvle.setStringCondition(rvle_handle, "cond", "package", pkg);
    rvle.setStringCondition(rvle_handle, "cond", "vpz", file);
    return(rvle_handle);
}

#'
#' init the embedded simulator
#' 
#' @export
#' 
#' @param rvle_handle, any rvle_handle built from rvle.open
#' @param all, a boolean that tells if a full check is performed
#'   
#' @return TRUE if the rvle_handle is built from vle.recursive.init 
#' (and configuration is ok if all=TRUE), false otherwise  
#' 
#' @author Ronan Trépos MIA-T, INRA
#' 
#' @note
#' 
#' @examples
#' 
#' f = vle.recursive(pkg="mypkg", file="mymodel.vpz")
#' vle.recursive.check(f)
#'
vle.recursive.check = function(rvle_handle=NULL, all=FALSE)
{
    if (is.null(rvle_handle)) {
        return (FALSE)
    }
    if (class(rvle_handle) != "rvle") {
        return (FALSE);
    }
    listCond = rvle.listConditions(rvle_handle);
    if (sum(listCond == "cond") ==0) {
        return (FALSE);
    }
    listPorts = rvle.listConditionPorts(rvle_handle, "cond");
    if (sum(listPorts == "package") ==0) {
        return (FALSE);
    }
    if (sum(listPorts == "vpz") ==0) {
        return (FALSE);
    }
    
    if (all) {
        #TODO check config
        return (FALSE);
    }
    return (TRUE);
}


#'
#' get a Rvle Object of the ebmedded model
#' 
#' @export
#'
#' @param rvle_handle, a rvle handle built with vle.recursive.init
#' @param replicate, index of the replicate for embedded
#'                   model configuration
#' 
#' @return a rvle handle built the embedded simulator
#' 
#' @author Ronan Trépos MIA-T, INRA
#' 
#' @return return the embedded model
#' 
#' @note
#' 
#' Will build an new rvle handle based on the embedding handle
#' 
#' @examples
#' 
#' f = vle.recursive(pkg="mypkg", file="mymodel.vpz");
#' femb = vle.recursive.getEmbedded (f);
#'  
vle.recursive.getEmbedded = function(rvle_handle=NULL, replicate=1)
{
    if (! vle.recursive.check(rvle_handle)) {
        stop("[vle.recursive] Error: rvle_handle is malformed");
        return (NULL);
    }
    ftmp = rvle.open(
            pkg=rvle.getConditionPortValues(rvle_handle, "cond", "package"),
            file=rvle.getConditionPortValues(rvle_handle, "cond", "vpz"));
    for (cond in rvle.listConditions(rvle_handle)) {
        for (port in rvle.listConditionPorts(rvle_handle, cond)) {
            if (startsWith(port, "propagate_")) {
                econdPort = strsplit(port, split="propagate_")[[1]][2]
                econd = strsplit(econdPort, split="\\.")[[1]][1]
                eport = strsplit(econdPort, split="\\.")[[1]][2]
                rvle.setValueCondition(ftmp, econd, eport,
                        rvle.getConditionPortValues(rvle_handle, cond, port));
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
#' show the content of the embedding simulator
#' 
#' @export
#'
#' @param rvle_handle, a rvle handle built with vle.recursive.init
#' 
#' @return NULL
#' 
#' @author Ronan Trépos MIA-T, INRA
#' 
#' @note
#' 
#' @examples
#' 
#' f = vle.recursive(pkg="mypkg", file="mymodel.vpz")
#' vle.recursive.show(f);
#' 
#'  
vle.recursive.show = function(rvle_handle=NULL)
{
    rvle.show(rvle_handle);
}

#'
#' show the content of the embedded model
#' 
#' @export
#'
#' @param rvle_handle, a rvle handle built with vle.recursive.init
#' 
#' @return NULL
#' 
#' @author Ronan Trépos MIA-T, INRA
#' 
#' @note
#' 
#' Will first build the embedded model and show its conditions
#' 
#' @examples
#' 
#' f = vle.recursive(pkg="mypkg", file="mymodel.vpz")
#' vle.recursive.showEmbedded(f);
#' 
#'  
vle.recursive.showEmbedded = function(rvle_handle=NULL)
{
  ftmp = vle.recursive.getEmbedded(rvle_handle);
  rvle.show(ftmp);
  rm(ftmp);
}

#'
#' init one input
#' 
#' @export
#'
#' @param rvle_handle, a rvle handle built with vle.recursive.init
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
#' f = vle.recursive(pkg="mypkg", file="mymodel.vpz")
#' vle.recursive.configPropagate(f, "mycond.myport", value=0.5)
#'  

vle.recursive.configPropagate = function(rvle_handle=NULL, propagate=NULL, 
                                         value=NULL)
{
  if (! vle.recursive.check(rvle_handle)) {
      stop("[vle.recursive] Error: rvle_handle is malformed");
      return (NULL);
  }
  inputPort = paste("propagate", sep="_", propagate);
  
  listPorts = rvle.listConditionPorts(rvle_handle, "cond");
  if (sum(listPorts == inputPort) ==0) {
    rvle.addPort(rvle_handle, "cond", inputPort);
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
  rvle.setValueCondition(rvle_handle, cond="cond", port=inputPort, 
                         value);
  
  #remove replicate and input with same name
  inputPort = paste("replicate", sep="_", propagate);
  listPorts = rvle.listConditionPorts(rvle_handle, "cond");
  if (sum(listPorts == inputPort) != 0) {
    rvle.removePort(rvle_handle, "cond", inputPort);
  }
  inputPort = paste("input", sep="_", propagate);
  listPorts = rvle.listConditionPorts(rvle_handle, "cond");
  if (sum(listPorts == inputPort) != 0) {
    rvle.removePort(rvle_handle, "cond", inputPort);
  }
}


#'
#' init one input
#' 
#' @export
#' 
#' @param rvle_handle, a rvle handle built with vle.recursive.init
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
#' f = vle.recursive(pkg="mypkg", file="mymodel.vpz")
#' vle.recursive.configInput(f, "mycond.myport", value=c(0.5, 0.6, 0.7))
#'  
    
vle.recursive.configInput = function(rvle_handle=NULL, input=NULL, values=NULL)
{
  if (! vle.recursive.check(rvle_handle)) {
      stop("[vle.recursive] Error: rvle_handle is malformed");
      return (NULL);
  }
  inputPort = paste("input", sep="_", input);
  
  listPorts = rvle.listConditionPorts(rvle_handle, "cond");
  if (sum(listPorts == inputPort) ==0) {
    rvle.addPort(rvle_handle, "cond", inputPort);
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
  rvle.setValueCondition(rvle_handle, cond="cond", port=inputPort, 
            values);

  #remove replicate and propagate with same name
  inputPort = paste("replicate", sep="_", input);
  listPorts = rvle.listConditionPorts(rvle_handle, "cond");
  if (sum(listPorts == inputPort) != 0) {
    rvle.removePort(rvle_handle, "cond", inputPort);
  }
  inputPort = paste("propagate", sep="_", input);
  listPorts = rvle.listConditionPorts(rvle_handle, "cond");
  if (sum(listPorts == inputPort) != 0) {
    rvle.removePort(rvle_handle, "cond", inputPort);
  }
}

#'
#' init the replicate condition port
#' 
#' @export
#' 
#' @param rvle_handle, a rvle handle built with vle.recursive.init
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

vle.recursive.configReplicate = function(rvle_handle=NULL, replicate=NULL, values=NULL)
{
  if (! vle.recursive.check(rvle_handle)) {
      stop("[vle.recursive] Error: rvle_handle is malformed");
      return (NULL);
  }
  inputPort = paste("replicate", sep="_", replicate);
  listPorts = rvle.listConditionPorts(rvle_handle, "cond");
  if (sum(listPorts == inputPort) ==0) {
    rvle.addPort(rvle_handle, "cond", inputPort);
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
  rvle.setValueCondition(rvle_handle, cond="cond", port=inputPort, 
                         values);
  
  #remove input and propagate with same name
  inputPort = paste("input", sep="_", replicate);
  listPorts = rvle.listConditionPorts(rvle_handle, "cond");
  if (sum(listPorts == inputPort) != 0) {
    rvle.removePort(rvle_handle, "cond", inputPort);
  }
  inputPort = paste("propagate", sep="_", replicate);
  listPorts = rvle.listConditionPorts(rvle_handle, "cond");
  if (sum(listPorts == inputPort) != 0) {
    rvle.removePort(rvle_handle, "cond", inputPort);
  }
}

#'
#' Get value of either an input a replicate a propagate or an embedded value
#' 
#' @export
#' 
#' @param rvle_handle, a rvle handle built with vle.recursive.init
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
vle.recursive.getValue = function(rvle_handle=NULL, id=NULL)
{
  if (! vle.recursive.check(rvle_handle)) {
      stop("[vle.recursive] Error: rvle_handle is malformed");
      return (NULL);
  }
  inputPort = paste("input", sep="_", id);
  ##try to find into rvle_handle an input
  if (length(which(rvle.listConditionPorts(rvle_handle,"cond") == 
                   inputPort)) > 0) {
    return(rvle.getConditionPortValues(rvle_handle, 
                                       "cond", inputPort));
  }
  
  inputPort = paste("replicate", sep="_", id);
  ##try to find into rvle_handle a replicate
  if (length(which(rvle.listConditionPorts(rvle_handle,"cond") == 
                   inputPort)) > 0) {
    return(rvle.getConditionPortValues(rvle_handle, 
                                       "cond", inputPort));
  }
  
  inputPort = paste("propagate", sep="_", id);
  ##try to find into rvle_handle a propagate
  if (length(which(rvle.listConditionPorts(rvle_handle,"cond") == 
                   inputPort)) > 0) {
    return(rvle.getConditionPortValues(rvle_handle, 
                                       "cond", inputPort));
  }

  ##try to find into embedded sim
  ftmp = rvle.open(
          pkg=rvle.getConditionPortValues(rvle_handle, "cond", "package"),
          file=rvle.getConditionPortValues(rvle_handle, "cond", "vpz"));
  econd = strsplit(id, split="\\.")[[1]][1]
  eport = strsplit(id, split="\\.")[[1]][2]
  return (rvle.getConditionPortValues(ftmp, econd, eport));
}

#'
#' config one output
#' 
#' @export
#' 
#' @param rvle_handle, a rvle handle built with vle.recursive.init
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
vle.recursive.configOutput = function(rvle_handle=NULL, id=NULL, path=NULL, 
        mse_observations=NULL, mse_times=NULL, integration="all", 
        aggregation_input="all")
{
    if (! vle.recursive.check(rvle_handle)) {
        stop("[vle.recursive] Error: rvle_handle is malformed");
        return (NULL);
    }

    outputPort = paste("output", sep="_", id);
    listPorts = rvle.listConditionPorts(rvle_handle, "cond");
    if (sum(listPorts == outputPort) == 0) {
        rvle.addPort(rvle_handle, "cond", outputPort);
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
    rvle.setValueCondition(rvle_handle, cond="cond", port=outputPort, 
            config);
#    rvle.addObservablePort(rvle_handle, "obs", id)
#    rvle.attachView(rvle_handle, "view", "obs", id)
}

#'
#' config the type of simulations
#' 
#' @export
#' 
#' @param rvle_handle, a rvle handle built with vle.recursive.init
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
#' @param expe_seed. Seed for the rng
#' @param expe_log. level of log (between 1 and 7) 
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

vle.recursive.configSimulation = function(rvle_handle=NULL, config_parallel_type=NULL, 
        config_parallel_rm_files=NULL, config_parallel_nb_slots=NULL, 
        working_dir=NULL, expe_seed = 12369, expe_log = 7)
{
    if (! vle.recursive.check(rvle_handle)) {
        stop("[vle.recursive] Error: rvle_handle is malformed");
        return (NULL);
    }

    listPorts = rvle.listConditionPorts(rvle_handle, "cond");
    if (sum(listPorts == "config_parallel_type") == 0) {
        rvle.addPort(rvle_handle, "cond", "config_parallel_type");
    }
    if (sum(listPorts == "config_parallel_rm_files") == 0) {
        rvle.addPort(rvle_handle, "cond", "config_parallel_rm_files");
    }
    if (sum(listPorts == "config_parallel_nb_slots") == 0) {
        rvle.addPort(rvle_handle, "cond", "config_parallel_nb_slots");
    }
    if (sum(listPorts == "working_dir") == 0) {
        rvle.addPort(rvle_handle, "cond", "working_dir");
    }
    if (sum(listPorts == "expe_seed") == 0) {
        rvle.addPort(rvle_handle, "cond", "expe_seed");
    }
    if (sum(listPorts == "expe_log") == 0) {
        rvle.addPort(rvle_handle, "cond", "expe_log");
    }
    if (is.null(config_parallel_type)) {
        rvle.setValueCondition(rvle_handle, cond="cond",
                port="config_parallel_type", "single");
    } else {
        rvle.setValueCondition(rvle_handle, cond="cond",
                port="config_parallel_type", config_parallel_type);
    }
    if (is.null(config_parallel_rm_files)) {
        rvle.setValueCondition(rvle_handle, cond="cond",
                port="config_parallel_rm_files", FALSE);
    } else {
        rvle.setValueCondition(rvle_handle, cond="cond",
                port="config_parallel_rm_files", config_parallel_rm_files);
    }
    if (is.null(config_parallel_nb_slots)) {
        rvle.setValueCondition(rvle_handle, cond="cond",
                port="config_parallel_nb_slots", as.integer(1));
    } else {
        rvle.setValueCondition(rvle_handle, cond="cond",
                port="config_parallel_nb_slots",
                as.integer(config_parallel_nb_slots));
    }
    if (is.null(working_dir)) {
        rvle.setValueCondition(rvle_handle, cond="cond",
                port="working_dir", "/tmp/");
    } else {
        rvle.setValueCondition(rvle_handle, cond="cond",
                port="working_dir", working_dir);
    }
    rvle.setValueCondition(rvle_handle, cond="cond",
            port="expe_log", as.integer(expe_log));

    rvle.setValueCondition(rvle_handle, cond="cond",
            port="expe_seed", as.integer(expe_seed));
}


#'
#' simulates the experiment plan
#' 
#' @export
#' 
#' @param rvle_handle, a rvle handle built with vle.recursive.init
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

vle.recursive.simulate = function(rvle_handle=NULL)
{
  if (! vle.recursive.check(rvle_handle)) {
      stop("[vle.recursive] Error: rvle_handle is malformed");
      return (NULL);
  }

  res = rvle.run(rvle_handle)$view$"vle-recursive:vle_recursive.outputs"[[1]];
  if (is.null(res)) {
    savedVpz = paste(Sys.getenv("VLE_HOME"),sep="/",
                     "pkgs-2.0/vle.recursive/exp/test_error.vpz"); 
    rvle.save(rvle_handle, savedVpz);
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
