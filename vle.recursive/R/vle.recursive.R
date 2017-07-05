
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
  if (! is.character(dateStr)) {
    return(NA);
  } 
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
#' f = vle.recursive.init(pkg="mypkg", file="mymodel.vpz")
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
#' check the rvle_handle
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
#' check a name is of  the form cond.port
#' 
#' @export
#' 
#' @param name, any char
#'   
#' @return TRUE if the name ifs of the form cond.port
#' 
#' @author Ronan Trépos MIA-T, INRA
#' 
#' @note
#' 
#' @examples
#' TODO
#'
vle.recursive.checkCondPortName = function(name=NULL) 
{
  resPlit = strsplit(name, split="\\.")[[1]];
  if (length(resPlit) != 2) {
     stop(paste("[vle.recursive] Error: cond.port malformed", name));
     return (FALSE);
  }
  return (TRUE)
}


#'
#' get a Rvle Object of the ebmedded model
#' 
#' @export
#'
#' @param rvle_handle, a rvle handle built with vle.recursive.init
#' @param input, index of the input for embedded model configuration
#' @param replicate, index of the replicate for embedded model configuration
#' 
#' @return a rvle handle built from the embedding model
#' 
#' @author Ronan Trépos MIA-T, INRA
#' 
#' @return return the embedded model
#' 
#' @note
#' 
#' Will build an new rvle handle based on the embedding model
#' 
#' @examples
#' 
#' f = vle.recursive(pkg="mypkg", file="mymodel.vpz");
#' femb = vle.recursive.getEmbedded (f);
#'  
vle.recursive.getEmbedded = function(rvle_handle=NULL, input=1, replicate=1)
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
                econdPort = strsplit(port, split="input_")[[1]][2]
                econd = strsplit(econdPort, split="\\.")[[1]][1]
                eport = strsplit(econdPort, split="\\.")[[1]][2]
                valSet =  rvle.getConditionPortValues(rvle_handle, cond, port);
                if ((class(valSet) == "VleSET")||(class(valSet) == "VleTUPLE")) {
                   rvle.setValueCondition(ftmp, econd, eport,valSet[[input]]);
                }     
            } else if (startsWith(port, "replicate_")) {
                econdPort = strsplit(port, split="replicate_")[[1]][2]
                econd = strsplit(econdPort, split="\\.")[[1]][1]
                eport = strsplit(econdPort, split="\\.")[[1]][2]
                valSet =  rvle.getConditionPortValues(rvle_handle, cond, port);
                if ((class(valSet) == "VleSET")||(class(valSet) == "VleTUPLE")) {
                   rvle.setValueCondition(ftmp, econd, eport,valSet[[replicate]]);
                }  
            }
        }
    }
    return(ftmp);
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
  vle.recursive.checkCondPortName(propagate)
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
  vle.recursive.checkCondPortName(input)
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
  vle.recursive.checkCondPortName(replicate)
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
#' config one output
#' 
#' @export
#' 
#' @param rvle_handle, a rvle handle built with vle.recursive.init
#' @param id, the id (a string) of the output
#' @param path, a string of the form  'viewname/pathOfTheAtomicModel.ObsPort'
#' that identifies the column to get from the simulation of the embedded
#' simulator
#' @param integration, amongst 'last', 'max', 'mse' or 'all' (default = 'last')
#' @param aggregation_replicate, amongst 'mean', 'quantile' (default = 'mean')
#' @param aggregation_input, amongst 'mean', 'max' or 'all' (default = 'all')
#' @param mse_times, times of observations (required only if 
#' integration='mse')
#' @param mse_observations, list of observations (required only if 
#' integration='mse')
#' @param replicate_quantile, real: quantile order (required only if 
#' aggregation_replicate='quantile')
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
        integration=NULL, aggregation_replicate=NULL, aggregation_input=NULL, 
        mse_times=NULL, mse_observations=NULL, replicate_quantile=NULL)
{
    if (! vle.recursive.check(rvle_handle)) {
        stop("[vle.recursive] Error: rvle_handle is malformed");
        return (NULL);
    }

    outputPort = paste("output", sep="_", id);
    listPorts = rvle.listConditionPorts(rvle_handle, "cond");
    #remove existing output port
    if (sum(listPorts == outputPort) != 0) {
       rvle.removePort(rvle_handle, "cond", outputPort);
    }
    rvle.addPort(rvle_handle, "cond", outputPort);

    if (is.null(id) || is.null(path)) {
       stop("[vle.recursive.configOutput] missing 'id' or 'path'");
       return;
    }
    config = list(id=id, path=path);
    if (! is.null(integration)) {
      if (integration == "mse") {
	config = c(config, list(integration=integration, 
            mse_times = mse_times, mse_observations = mse_observations));
      } else {
        config = c(config, list(integration=integration));
      }
    }
    if (! is.null(aggregation_replicate)) {
       if (aggregation_replicate == "quantile") {
	config = c(config, list(aggregation_replicate=aggregation_replicate, 
            replicate_quantile = replicate_quantile));
      } else {
        config = c(config, list(aggregation_replicate=aggregation_replicate));
      }
    }
    if (! is.null(aggregation_input)) {
      config = c(config, list(aggregation_input=aggregation_input));
    }
    class(config) <- "VleMAP";
    rvle.setValueCondition(rvle_handle, cond="cond", port=outputPort, 
            config);
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
#' @param withSpawn, if true the simulation in spawned in another process
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

vle.recursive.simulate = function(rvle_handle=NULL, withSpawn=1)
{
  if (! vle.recursive.check(rvle_handle)) {
      stop("[vle.recursive] Error: rvle_handle is malformed");
      return (NULL);
  }

  res = rvle.run(rvle_handle, withSpawn=withSpawn);
  if (! is.null(res)) {
    res = res$view$"vle-recursive:vle_recursive.outputs"[[1]];
  }
  if (is.null(res) || is.character(res)) {
    savedVpz = paste(Sys.getenv("VLE_HOME"),sep="/",
                     "pkgs-2.0/vle.recursive/exp/test_error.vpz"); 
    rvle.save(rvle_handle, savedVpz);
    print(paste("[vle.recursive] Error:", res));
    stop(paste("see saved vpz:", savedVpz));
    rvlelog = paste(Sys.getenv("VLE_HOME"),sep="/", "rvle.log");
    if (file.exists(rvlelog)) {
      print(paste("[vle.recursive] rvle log file date: ", 
                        file.info(rvlelog)$mtime));
      cat(readLines(rvlelog),sep="\n")
    }
  }
  class(res) = "vle.recursive.simulate";
  return (res)
}

#'
#' Compute RMSE between simulations and obs
#' 
#'  @param rvle_handle, a rvle handle built with vle.recursive.init
#'  @param: file_sim,  filename of simulations
#'  @param: file_obs,  filename of observations
#'  @param: id,  id to simulate for comparison
#'  @param: typeReturn,  either 'all' or 'rmse'
#'  @return: a complex structure
#' 
vle.recursive.compareSimObs=function(rvle_handle=NULL, file_sim=NULL, file_obs=NULL, 
                                     id=NULL, typeReturn="all")
{
  #read observations
  output_vars = vle.recursive.readObservationsHeader(file=file_obs);
  output_vars_without_id = output_vars;
  output_vars_without_id[["id"]] <- NULL;
  observations = vle.recursive.readObservations(file=file_obs, 
                   output_vars=output_vars, withWarnings=TRUE)
  #read simulations
  simulations =  vle.recursive.readSimulations(file=file_sim, 
                         withWarnings=TRUE)
  
  id_sim = intersect(observations$id, simulations$id)
  if (! is.null(id)) {
   id_sim = intersect(id, id_sim);
  }  

  vle.recursive.setSimulations(rvle_handle=rvle_handle, sim=simulations,
       id=id_sim, withWarnings=TRUE);
  
  vle.recursive.setOutputs(rvle_handle=rvle_handle,
       output_vars=output_vars_without_id,
       integration='all');

  sim_res = vle.recursive.simulate(rvle_handle=rvle_handle);

  #recover simulations and observations
  obsValues = list();
  simValues = list();
  id_rmse = NULL;
  obs_dates = NULL;

  for (i in 1:length(id_sim)) {
    idI = id_sim[i]; 

    obsi = observations[which(observations$id == idI),];
    for (o in 1:nrow(obsi)) {
       dateo = obsi[o, "date"];
       indDateInSim = which(vle.recursive.dateFromNum(sim_res$date[,i]) == dateo)
       id_rmse = c(id_rmse, idI);     
       obs_dates = c(obs_dates, dateo); 
       for (v in setdiff(names(obsi), c("date", "id"))) {
         obsValues[[v]] = c(obsValues[[v]], obsi[[v]][o])
         simValues[[v]] = c(simValues[[v]], sim_res[[v]][indDateInSim,i])
       }
    }
  }
  #compute rmse
  for (v in names(obsValues)) {
    squareError = (obsValues[[v]] - simValues[[v]])^2;
    elOk = !is.na(squareError);
    print(paste(sep="", " rmse '",v,"' : ", 
                   sqrt(sum(squareError[elOk]/sum(elOk))))); 

  }
  ret = NULL;
  if (typeReturn == 'rmse') {
    ret = list(id_rmse=id_rmse, obsValues=obsValues, simValues=simValues);
  } else {
    ret = list(id_rmse=id_rmse, id_sim=id_sim, obs_dates=obs_dates, obsValues=obsValues,
               simValues=simValues, sim_res = sim_res);
  }
  
  return(ret);
}


#'
#' Performs a sensitivy analysis
#'
#' @param rvle_handle, a rvle handle built with vle.recursive.init
#' @param: file_as, either a filename of a csv file or a dataframe
#'     column1 Param: gives the parameter (with form cond.port)
#'     column2 Value: gives the default value 
#'     column3 min: gives the minimal value (or NA) if it does not vary
#'     column3 max: gives the maximal value (or NA) if it does not vary
#' @param output_vars, list of named path eg c(LAI="view/Coupled:Atomic.port")
#'     on which the sensitivity analysis is performed
#' @param integration, type of integration for outputs (default 'last')
#' @param r, number of replicate of the morris method
#' 
#' usage:
#'  source("vle.recursive.R")
#'  f = vle.recursive.init(pkg="mypkg", file="mymodel.vpz")
#'  
#'
vle.recursive.sensitivity = function(rvle_handle=rvle_handle, file_as=NULL,
                   output_vars=NULL, integration=NULL, r=100)
{
  library(sensitivity)
  as_bounds = read.table(file_as, sep=";", header=T, stringsAsFactors=F,
          row.names=c("default", "min","max"));
  as_bounds$parameter <- NULL;
  bounds=NULL;
  bounds_col_names=NULL;
  bounds_row_names=c("min", "max");
  #set propagate and define bounds
  for (i in 2:ncol(as_bounds)) {
    
    if (is.na(as_bounds["min",i])) {
      #define propagate
      vle.recursive.configPropagate(rvle_handle=rvle_handle, 
          propagate=names(as_bounds)[i], 
          value=as_bounds["default",i]);
    } else {
      #define bound
      bounds = cbind(bounds, c(as_bounds["min",i], as_bounds["max",i])); 
      bounds_col_names = c(bounds_col_names, names(as_bounds)[i]);
    }
  }
  bounds = as_bounds[, which(! is.na(as_bounds["min", ]))]
  #convert date to int if required
  for (i in colnames(bounds)){
    if (! is.na(vle.recursive.dateToNum(bounds["default", i]))) {
        bounds[, i] = as.integer(c(
                        vle.recursive.dateToNum(bounds["default", i]),
                        vle.recursive.dateToNum(bounds["min", i]),
                        vle.recursive.dateToNum(bounds["max", i])))
    }
  }

  #generate plan
  morris_res = morris(model=NULL, 
      factors = as.character(colnames(bounds)), 
      r = r, design = list(type="oat", levels=5, grid.jump=2),
      binf=as.numeric(bounds["min",]), bsup=as.numeric(bounds["max",]));

  #config simulator with exp plan
  for (i in 1:ncol(bounds)){
     vle.recursive.configInput(rvle_handle=rvle_handle, 
          input=colnames(bounds)[i], 
          values=morris_res$X[,i]);
  }
  vle.recursive.setOutputs(rvle_handle=rvle_handle, output_vars=output_vars,
       integration=integration);
  res = vle.recursive.simulate(rvle_handle);
  res_sensitivity = list();
  for (i in names(output_vars)) {
    morris_res_bis = morris_res;
    tell(morris_res_bis, res[[i]][1,])
    res_sensitivity[[i]] <- morris_res_bis
  }
  return(res_sensitivity)  
}

#'
#' set parameters for multisimulations of vle models from a dataframe 
#' or csv file
#'
#' @param rvle_handle, a rvle handle built with vle.recursive.init
#' @param: sim, either a filename of a csv file or a dataframe
#' @param: id [optionnal], vector of int giving the simulations id to perform
#' @param: withWarnings [optionnal], if true, warning are given while 
#'                                   reading data
#' @return: eg. a names list (names ar vars) of matrices 
#'               of nb simu rows and time columns
#' 
#' usage:
#'  source("vle.recursive.R")
#'  f = vle.recursive.init(pkg="mypkg", file="mymodel.vpz")
#'  vle.recursive.setSimulations(rvle_handle=f,, 
#'                   sim="simu_file.csv"); 
#'  r = vle.recursive.simulate(f)
#'  
#'
vle.recursive.setSimulations = function(rvle_handle=NULL, sim=NULL,
                                        id=NULL, withWarnings=TRUE)
{
  #read inputs
  inputs=NULL;
  if (is.character(sim)) {
    inputs = vle.recursive.readSimulations(sim, withWarnings=withWarnings);
  } else {
    inputs = sim;
  }

  #keep only line to simulate
  if (! is.null(id)) {
    if (! all(id %in% inputs$id)) {
       stop("[vle.recursive] requiring to simulate id that does not exist");
    }
    inputs = inputs[match(id,inputs$id),];
  }
  #config inputs
  for (input in names(inputs)) {
     if (input != "id") {
       if (nrow(inputs) > 1) {
         vle.recursive.configInput(rvle_handle=rvle_handle, 
                               input=input, values=inputs[[input]]); 
       } else {
         vle.recursive.configPropagate(rvle_handle=rvle_handle, 
                               propagate=input, value=inputs[[input]]); 
       }
     }
  }
}


#'
#' Set outputs for simulation
#' 
#' @export
#' 
#' @param rvle_handle, a rvle handle built with vle.recursive.init
#' @param output_vars, a list of named characher eg. c(LAI="view/Coupled:atomic.port)
#'        each one identifies an output, names are output ids and content are paths
#' @param integration, list of integrations type amongst 'last', 'max'
#'        or 'all' (default = 'last') of size length(output_vars)
#' @param aggregation_input, list of aggregation types amongst 'mean', 'max' 
#'        or 'all' (default = 'all') of size length(output_vars)
#' 
#' @return NULL
#' 
#' @author Ronan Trépos MIA-T, INRA
#' 
#' @note
#' 
#' all configurations cannot be given with this function (aggregation_replicate, mse,
#' quantile)
#' 
#' @examples
#' 
#' #TODO
#' 
vle.recursive.setOutputs = function(rvle_handle=NULL, output_vars=NULL,  
        integration=NULL, aggregation_input=NULL)
{
   if (is.null(integration)) {
     integration = rep('last', length(output_vars));
   } else if (length(integration) == 1) {
     integration = rep(integration, length(output_vars));
   }
   if (is.null(aggregation_input)) {
     aggregation_input = rep('all', length(output_vars));
   } else if (length(aggregation_input) == 1) {
     aggregation_input = rep(aggregation_input, length(output_vars));
   }
   i = 0;
   for (var in names(output_vars)) {
     i = i+1;
     vle.recursive.configOutput(rvle_handle=rvle_handle, id=var,
       path=output_vars[[var]], integration=integration[i], 
       	aggregation_input=aggregation_input[i]);
   } 
}


#'
#' Reading of the simulation file if necessary and pretraitement of data
#' 
#'  @param: file, a filename of input combinations
#'  @param: keepID, if true the 'id' column is kept
#'  @param: withWarnings, if true warnings are given on missing data
#'  @return: a data.frame of simulations to perform
#' 
vle.recursive.readSimulations = function(file=NULL, keepID=TRUE, withWarnings=TRUE)
{
  #read inputs
  inputs = read.table(file, sep=";", skip=1, header=TRUE, 
            stringsAsFactors = FALSE);
  #remove useless columns
  headerNames = names(inputs)
  for (n in headerNames) {
    if (substring(n,1,1) == "X") { #remove it
      inputs[[n]] <- NULL;
    } else if ((n == "id") && ! keepID){
      inputs[[n]] <- NULL;
    } else if (all(is.na(inputs[[n]]))){
      inputs[[n]] <- NULL;
      if (withWarnings) {
        warning(paste("Problem in simulation data: Na in all column", n, 
                      ", column is removed"));  
      }
    } else if (sum(is.na(inputs[[n]])) > 1) {
      if (withWarnings) {
        warning(paste("Problem  in simulation data: Na in column", n,
                      ", missing lines are removed"));
      }
      inputs = inputs[!is.na(inputs[[n]]), ];
    }
  }
  return(inputs)
}


#'
#' Reading of the observations file looking for output headers
#'
#' @export
#' 
#' @param: file,  filename of observations
#' 
#' @return a list of named characher eg. c(LAI="view/Coupled:atomic.port)
#'  each one identifies an output
#' 
vle.recursive.readObservationsHeader = function(file=NULL)
{
  obss = read.table(file, sep=";", skip=1, header=TRUE, nrow=1,
            stringsAsFactors = FALSE);

  #remove 'X\\..*'
  toremove = grep ("X\\.", names(obss));
  #remove 'X'
  toremove = c(toremove, which(names(obss) == "X"));
  if (length(toremove) > 0) {
    obss = obss[,-toremove];
  }
  obss = as.list(obss);
  return(obss)
}

#'
#' Reading of the observations file if necessary and pretraitement of data
#' 
#'  @param:file,  filename of observations
#'  @param output_vars, the output variables as computed by readObservationsHeader
#'  @param withWarnings, if true, send warnings when missing data
#'  @return: a data.frame of observations
#' 
vle.recursive.readObservations = function(file=NULL, output_vars=NULL, withWarnings=TRUE)
{
  
  header = read.table(file, sep=";", skip=1, nrow=1, header=FALSE, 
            stringsAsFactors = FALSE);
  tokeep = !is.na(header)
  header = header[tokeep]

  if ((length(header) != length(names(output_vars))) || 
      (!all.equal(header, names(output_vars)))) {
    stop("[vle.recursive] In observations file output_vars does not match file header");
  }

  obss = read.table(file, sep=";", skip=3, header=FALSE, 
            stringsAsFactors = FALSE);

  obss = as.data.frame(obss[, tokeep])
  colnames(obss) <- header;

  #remove useless columns and lines of obss
  for (n in header) {
    if (all(is.na(obss[[n]]))){
      obss[[n]] <- NULL;
      if (withWarnings){
          warnings(paste("remove column because all are NA:", n))
      }
    }
  }
  toremove = NULL;
  for (r in 1:nrow(obss)) {
      if (all(is.na(obss[r,]))) {
          toremove = c(toremove, r);
          if (withWarnings){
            warnings(paste("remove row because all are NA:", r))
          }
      }
  }
  if (! is.null(toremove)) {
      obss = obss[-toremove,];    
  }
  return(obss)
}


#'
#' Generic function for plot
#' 
#'  @param: obj, an R object of type compareSimObs or vle.recursive.simulate
#'  @param: output_vars, list of output_vars
#'  @param: id, ids to plots (for compareSimObs only)
#'  @param: seq, sequence of simulation to plot (for vle.recursive.simulate only)
#' 
vle.recursive.plot = function(obj=NULL, output_vars=NULL, id=NULL, seq=NULL) 
{
   library(gridExtra)
   library(grid)
   library(ggplot2)
   library(reshape2)
   
   #try to simulate results of vle.recursive.simulate
   if (class(obj) == "vle.recursive.simulate"){
     if (is.null(output_vars)) {
       stop("[vle.recursive] output_vars should be given ");
     }
     if (! is.null(id)) {
       stop("[vle.recursive] id should not be given ");
     }
     gpAll = NULL;
     i = 1;
     for (var in output_vars) {
       sel = 1:ncol(obj[[var]]);
       
       if (! is.null(seq)) {
         sel = seq;
       }
       gp = ggplot(data=melt(t(obj[[var]][,sel])), 
            aes(x=Var2, group=Var1, y=value)) + geom_line()+
            labs(x="time", y=var); 
       gpAll[[i]] = gp;
       i = i+1;
     }
     do.call(grid.arrange, gpAll);
     return (invisible(NULL))
   }
   
   #try to plot compareSimObs
   if (! is.list(obj)) {
     stop("[vle.recursive] obj is not recognized ");
   }

   #identify what to plot 
   what = NULL;
   if (is.null(id)) {
     what = "sim_vs_obs";
   } else if (length(id) > 1) {
     what = "sim_vs_obs";
   } else {
     what = "specific_id";
   }
   

   #identify vars
   vars = NULL;
   if (is.null(output_vars)) {
     if (! is.null(obj$obsValues)) {
       vars = names(obj$obsValues);
     }
   } else {
     if (is.null(names(output_vars))) {
       vars = output_vars;
     } else {
       vars = names(output_vars);
     }
   }

   #plot sim obs
   if ((what == 'sim_vs_obs') && 
       all (c("id_rmse", "obsValues", "simValues") %in% names(obj))) {
     gpAll = NULL;
     i = 1;
     for (var in vars) {
        df = data.frame(obs=obj$obsValues[[var]], sim=obj$simValues[[var]], 
                        id=obj$id_rmse)
        gp = ggplot(data=subset(subset(df, !is.na(obs)), !is.na(sim)), 
                 aes(x=obs, y=sim, label=id))+geom_text() +
                 geom_abline(intercept = 0, slope = 1) +
                 labs(x=paste("obs", var), y=paste("sim", var));
        gpAll[[i]] = gp;
        i = i+1;
     }
     do.call(grid.arrange, gpAll);
     return (invisible(NULL))
   }
   #plot dynamic
   if ((what == 'specific_id')  && 
       all (c("id_rmse", "sim_res", "obs_dates", "id_sim", "obsValues") 
       %in% names(obj))) {
     gpAll = NULL;
     i = 1;

     for (var in vars) {
       df = data.frame(
           sim=obj$sim_res[[var]][,which(obj$id_sim == id)],
           obs=as.numeric(NA))
       df$time = 1:nrow(df)
       df$date = obj$sim_res[["date"]][,which(obj$id_sim == id)]
       indSim = df$date %in% vle.recursive.dateToNum(
                     obj$obs_dates[which(obj$id_rmse == id)]);
       df$obs[indSim] = obj$obsValues[[var]][which(obj$id_rmse == id)]         
       gp = ggplot(data=df)+
               geom_line(aes(x=time, y=sim))+
               geom_point(aes(x=time, y=obs))+
               labs(x="time", y=paste("sim", var));
       gpAll[[i]] = gp;
       i = i+1;
     }
     do.call(grid.arrange, gpAll);
     return (invisible(NULL))
   }
   
}



