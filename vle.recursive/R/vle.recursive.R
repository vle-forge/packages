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

vle.recursive.dateToNum = function(dateStr)
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
#' Get available outputs in embedded simulator
#' 
#' @export
#' 
#' @param rvle_handle, a rvle handle built with vle.recursive.init
#' 
#' @return list of type c(Port="view/Coupled:Atom.Port", ..)
#' 
#' @author Ronan Trépos MIA-T, INRA
#' 
#' @note
#' 
#' Simulation is required
#' 
#' @examples
#' 
#' f = vle.recursive(pkg="mypkg", file="mymodel.vpz")
#' outputsAvail = vle.recursive.getAvailOutputs(f);
#'  
vle.recursive.getAvailOutputs = function(rvle_handle=NULL)
{
  if (! vle.recursive.check(rvle_handle)) {
      stop("[vle.recursive] Error: rvle_handle is malformed");
      return (NULL);
  }
  ff = vle.recursive.getEmbedded(rvle_handle);
  for (v in rvle.listViews(ff)) {
    rvle.setOutputPlugin(ff, viewname=v, plugin="vle.output/storage");
  }
  res = rvle.run(ff);
  outs = NULL
  ports = NULL
  for (v in names(res)) {
    for (absPort in names(res[[v]])) {
      if (absPort != "time") {
        port = strsplit(absPort, split="\\.")[[1]][2];
        ports =c(ports, port);
        outs = c(outs, paste(sep="/",v, absPort));
      }
    }
  }
  names(outs)=ports;
  return (as.list(outs));
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
#' Set outputs for simulation
#' 
#' @export
#' 
#' @param rvle_handle, a rvle handle built with vle.recursive.init
#' @param output_vars, a list of named characher eg. 
#'          c(portname="view/Coupled:atomic.port)
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
vle.recursive.configOutputs = function(rvle_handle=NULL, output_vars=NULL,  
        integration=NULL, aggregation_input=NULL)
{
  if (sum(duplicated(names(output_vars))) > 0){
    stop(paste(sep="", "[vle.recursive] Error: duplicated output:'",
               names(output_vars)[which(duplicated(names(output_vars))[1])],
               "'"));
    return (NULL);
  }
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
#' Parses an experiment configuration file
#'
#' @param file_expe, either a filename or a dataframe providing bounds on
#'        inputs. Columns are of type 'cond.port', lines are 'default', 
#'        'min' and 'max'.
#' @param rvle_handle [optionnal], a rvle handle built with
#'        vle.recursive.init, default values are used to initialize the model
#' @param typeReturn [default:'all'], either 'all' or 'bounds'
#'        it defines the obect to return
#' @param skip [default:0], skip parameter of read.table
#' @return either:
#'    - the bounds min max of the experiment if typeReturn == 'bounds'
#'    - the all dataframe (with default values) if typeReturn == 'all'
#'
#' usage:
#'  source("vle.recursive.R")
#'  f = vle.recursive.init(pkg="mypkg", file="mymodel.vpz")
#'  vle.recursvice(file_expe="myexperiment.csv", rvle_handle=f)
#'  
#'
vle.recursive.parseExpe = function(file_expe=NULL, rvle_handle=NULL,
 typeReturn='all', skip=0)
{
  if (is.character(file_expe)) {
    file_expe = read.table(file_expe, sep=";", header=T, stringsAsFactors=F,
          skip=skip, row.names=c("default", "min","max"));
  }
  expe = file_expe;
  expe$parameter <- NULL;
  bounds=NULL;
  bounds_col_names=NULL;
  #set propagate and define bounds
  for (i in 1:ncol(expe)) {
    #convert date to int if required
    if (! is.na(vle.recursive.dateToNum(expe["default", i]))) {
      if (is.na(expe["min",i])) {
        expe[, i] = as.numeric(c(
             vle.recursive.dateToNum(expe["default", i]), NA, NA));
      } else {    
        expe[, i] = as.numeric(c(
             vle.recursive.dateToNum(expe["default", i]), 
             vle.recursive.dateToNum(expe["min", i]),
             vle.recursive.dateToNum(expe["max", i])));
      }
    }
    #define propagate
    if (! is.null(rvle_handle)) {
      vle.recursive.configPropagate(rvle_handle=rvle_handle, 
             propagate=names(expe)[i], 
             value=expe["default",i]);
    }
    if (! is.na(expe["min",i])) {
      inputName = names(expe)[i];
      #define bound
      bounds = cbind(bounds, c(expe["min",i], expe["max",i])); 
      bounds_col_names = c(bounds_col_names, inputName);
    }
  }
  bounds = data.frame(bounds, row.names=c("min", "max"));
  colnames(bounds) <- bounds_col_names
  if (typeReturn == 'all') {
    return (file_expe);
  } else {
    return (bounds);
  }
}

#'
#' Parses a simulation file
#'
#' @param file_sim, either a filename or a dataframe providing bounds on
#'        inputs. Columns are of type 'cond.port', lines are 'default', 
#'        'min' and 'max'.
#' @param rvle_handle [optionnal], a rvle handle built with
#'        vle.recursive.init, input values are used to initialize the model
#' @param withWarnings [default:TRUE], gives warnings if true
#' @param skip [default:1], skip parameter of read.table
#' @return the dataframe of intputs
#'
#' usage:
#'
#'  source("vle.recursive.R")
#'  f = vle.recursive.init(pkg="mypkg", file="mymodel.vpz")
#'  vle.recursive.parseSim(file_sim="mysimulations.csv", rvle_handle=f)
#'  
#'
vle.recursive.parseSim = function(file_sim=NULL, rvle_handle=NULL, id=NULL,
  withWarnings=TRUE, skip=1)
{
  #read inputs
  if (is.character(file_sim)) {
    file_sim = read.table(file_sim, sep=";", skip=skip, header=TRUE, 
            stringsAsFactors = FALSE); 
  }
  inputs = file_sim

  #remove useless columns
  headerNames = names(inputs)
  for (n in headerNames) {
    if (substring(n,1,1) == "X") { #remove it
      inputs[[n]] <- NULL;
    } else if (all(is.na(inputs[[n]]))){
      inputs[[n]] <- NULL;
      if (withWarnings) {
        warning(paste("Problem in simulation data: Na in all column", n, 
                      ", column is removed"));  
      }
    } else if (sum(is.na(inputs[[n]])) > 1) {
      if (withWarnings) {
        strId = paste(which(is.na(inputs[[n]])), collapse=",");
        warning(paste("Problem  in simulation data: Na in column", n,
                      ", missing lines are removed id=", 
                      strId));
      }
      inputs = inputs[!is.na(inputs[[n]]), ];
    }
  }

  #keep only line to simulate
  if (! is.null(id)) {
    if (! all(id %in% inputs$id) & withWarnings) {
       warnings("[vle.recursive] requiring to simulate id that does not exist");
    }
    inputs = inputs[which(is.element(inputs$id, id)),];
  }

  #config inputs to simulate
  if (! is.null(rvle_handle)) {
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
  return(inputs)
}



#'
#' Parses an observation file
#'
#' @export
#' 
#' @param: file_obs,  filename of observations
#' @param rvle_handle [optionnal], a rvle handle built with
#'        vle.recursive.init, input values are used to initialize the model
#' @param typeReturn [default:'all'], 'all' or 'header'
#' @param withWarnings [default:TRUE], gives warnings if true
#' @param skip [default:1], skip parameter of read.table
#' 
#' @return either 
#'   - a list of named characher eg. c(VAR="view/Coupled:atomic.port)
#'  each one identifies an output if typeReturn == 'header'
#'   - a dataframe of observations if typeReturn == 'all'
#' 
vle.recursive.parseObs = function(file_obs=NULL, rvle_handle=NULL, 
  typeReturn='all', withWarnings=TRUE, skip=1)
{
  #reader header
  obss_header = read.table(file_obs, sep=";", skip=skip, header=TRUE, nrow=1,
            stringsAsFactors = FALSE);

  #remove 'X\\..*'
  toremove = grep ("X\\.", names(obss_header));
  #remove 'X'
  toremove = c(toremove, which(names(obss_header) == "X"));
  if (length(toremove) > 0) {
    obss_header = obss_header[,-toremove];
  }
  obss_header = as.list(obss_header);
  header = obss_header;
  header[["id"]] <- NULL;

  #read content
  obss = read.table(file_obs, sep=";", skip=2+skip, header=FALSE, 
            stringsAsFactors = FALSE);
  if (length(toremove) > 0) {
    obss = obss[,-toremove];
  }
  colnames(obss) <- names(obss_header);
  
  #remove useless columns and lines of obss
  for (n in names(obss_header)) {
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

  #config output
  if (! is.null(rvle_handle)) {

    vle.recursive.configOutputs(rvle_handle=rvle_handle,
       output_vars=header, integration='all');
  }
  if (typeReturn == 'all') {
    return (obss);
  } else {
    return (header);
  }
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
  observations = vle.recursive.parseObs(file_obs=file_obs, rvle_handle=rvle_handle, 
                                        withWarnings=TRUE)

  #read simulations
  id_sim = unique(observations$id);
  if (! is.null(id)) {
    id_sim = intersect(id,observations$id);
  }
  simulations =  vle.recursive.parseSim(file_sim=file_sim, 
                     rvle_handle=rvle_handle, id=id_sim, withWarnings=TRUE)

  sim_res = vle.recursive.simulate(rvle_handle=rvle_handle);


  #recover simulations and observations
  obsValues = list();
  simValues = list();
  id_rmse = NULL;
  obs_dates = NULL;

  for (i in 1:length(simulations$id)) {
    idI = simulations$id[i]; 
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
    ret = list(id_rmse=id_rmse, id_sim=simulations$id, obs_dates=obs_dates, obsValues=obsValues,
               simValues=simValues, sim_res = sim_res);
  }
  
  return(ret);
}


#'
#' Performs a sensitivy analysis
#'
#' @param rvle_handle, a rvle handle built with vle.recursive.init
#' @param file_expe, a file_expe parameter of vle.recursive.parseExpe
#' @param output_vars, list of named path eg c(LAI="view/Coupled:Atomic.port")
#'     on which the sensitivity analysis is performed
#' @param integration, type of integration for outputs (default 'last')
#' @param method, either 'morris' or 'fast99' or 'sobolEff'
#' @param r, number of replicate of the morris method
#' @param levels, number levels of the morris method
#' @param n, sample size of fast99 method or sobolEff
#' 
#' usage:
#'  source("vle.recursive.R")
#'  f = vle.recursive.init(pkg="mypkg", file="mymodel.vpz")
#'  
#'
vle.recursive.sensitivity = function(rvle_handle=rvle_handle, file_expe=NULL,
                   output_vars=NULL, integration=NULL, method='morris', 
                   r=100, levels=5, n=100)
{
  library(sensitivity)
  bounds = vle.recursive.parseExpe(file_expe, rvle_handle, typeReturn='bounds');
  #generate plan
  sensi_plan = NULL;
  if (method == 'morris') {
    sensi_plan = morris(model=NULL, 
      factors = as.character(colnames(bounds)), 
      scale=TRUE, #warning!! this is required, see if one could directly
                  #simulate into [0;1]
      r = r, design = list(type="oat", levels=levels, grid.jump=1),
      binf=as.numeric(bounds["min",]), bsup=as.numeric(bounds["max",]));
  } else if (method == 'fast99'){
    bounds_f = NULL
    for (i in 1:ncol(bounds)) {
      bounds_f[[i]] = list(min=bounds["min",i], max=bounds["max",i])
    }

    sensi_plan = fast99(model = NULL, factors=names(bounds), n=n, q="qunif",
                       q.arg = bounds_f);
    if (sum(is.na(sensi_plan$omega)) > 1){
     stop("[vle.recursive] Error: fast99 not enough sims (?)");
    }
  } else if (method == 'sobolEff'){
    X1 = matrix(runif(ncol(bounds) * n), nrow = n);
    X2 = matrix(runif(ncol(bounds) * n), nrow = n);
    for (i in 1:ncol(bounds)) {
      X1[,i] = bounds["min",i] + X1[,i] * (bounds["max",i]- bounds["min",i]);
      X2[,i] = bounds["min",i] + X2[,i] * (bounds["max",i]- bounds["min",i]);
    }
    X1 = as.data.frame(X1);
    X2 = as.data.frame(X2);
    colnames(X1) <- colnames(bounds);
    colnames(X2) <- colnames(bounds);
    sensi_plan = sobolEff(model = NULL, X1=X1, X2=X2);
  }

  #config simulator with exp plan
  for (i in 1:ncol(bounds)){
     vle.recursive.configInput(rvle_handle=rvle_handle, 
          input=colnames(bounds)[i], 
          values=sensi_plan$X[,i]);
  }
  vle.recursive.configOutputs(rvle_handle=rvle_handle, output_vars=output_vars,
       integration=integration);
  res = vle.recursive.simulate(rvle_handle);
  res_sensitivity = list();
  for (i in names(output_vars)) {
    sensi_plan_bis = sensi_plan;
    tell(sensi_plan_bis, res[[i]][1,])
    res_sensitivity[[i]] <- sensi_plan_bis
  }
  return(res_sensitivity)  
}

#'
#' Performs simulation of a LHS
#'
#' @param rvle_handle, a rvle handle built with vle.recursive.init
#' @param file_expe, a file_expe parameter of vle.recursive.parseExpe
#' @param output_vars, list of named path eg c(LAI="view/Coupled:Atomic.port")
#'     for which the the simulation is performed
#' @param integration, type of integration for outputs (default 'last')
#' @param n, number of points to generate in LHS
#' @param typeReturn [default:'all'], either 'all' or 'y'
#'        it defines the obect to return
#' 
#' usage:
#'  source("vle.recursive.R")
#'  f = vle.recursive.init(pkg="mypkg", file="mymodel.vpz")
#'  
#'
vle.recursive.lhs = function(rvle_handle=rvle_handle, file_expe=NULL,
                   output_vars=NULL, integration=NULL, n=100, typeReturn='all')
{
  library(lhs)
  bounds = vle.recursive.parseExpe(file_expe, rvle_handle, typeReturn='bounds');
  #generate plan
  lhs_plan = randomLHS(n=n, k=ncol(bounds));

  #config simulator with exp plan
  for (i in 1:ncol(bounds)){
     pname = colnames(bounds)[i];
     vle.recursive.configInput(rvle_handle=rvle_handle, 
          input=pname, 
          values= bounds["min",pname] + 
                  lhs_plan[,i]*(bounds["max",pname]-bounds["min",pname]));
  }
  vle.recursive.configOutputs(rvle_handle=rvle_handle, output_vars=output_vars,
       integration=integration);
  res = vle.recursive.simulate(rvle_handle);
  
  #build input dataframe
  if (typeReturn == 'all') {
    X = NULL;
    for (i in 1:ncol(bounds)){
      pname = colnames(bounds)[i];
      X = cbind(X, bounds["min",pname] + 
                  lhs_plan[,i]*(bounds["max",pname]-bounds["min",pname]));
    }
    colnames(X) <- colnames(bounds);
    return(list(X=X, y=res));
  } else {
    return(res) 
  }
}



#'
#' Generic function for plot
#' 
#'  @param: obj, an R object of type compareSimObs or vle.recursive.simulate
#'  @param: output_vars, list of output_vars
#'  @param: id, ids to plots
#' 
vle.recursive.plot = function(obj=NULL, output_vars=NULL, id=NULL) 
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
     gpAll = NULL;
     i = 1;
     for (var in output_vars) {
       sel = 1:ncol(obj[[var]]);
       if (! is.null(id)) {
         sel = id;
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



