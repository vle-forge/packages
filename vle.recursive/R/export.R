

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
rvle.recursive.init = function(pkg, file)
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
#' @param input, a string of the form 'cond.port'
#' @param values, the set of values to simulate
#' @param replicate (optionnal), if TRUE the input is considered as a replicate
#' @param asSimple, values is considered as a simple value
#' (not an experiement plan)
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
    
rvle.recursive.configInput = function(input=NULL, values=NULL, 
        replicate=FALSE, asSimple=FALSE)
{
    if (is.null(rvle_recursive)) {
        print("[rvle.recursive] error missing initialization");
        return (NULL);
    }
    inputPort = NULL;
    if (replicate) {
        inputPort = paste("replicate", sep="_", input);
    } else {
        inputPort = paste("input", sep="_", input);
    }
    listPorts = rvle.listConditionPorts(rvle_recursive@sim, "cond");
    if (sum(listPorts == inputPort) ==0) {
        rvle.addPort(rvle_recursive@sim, "cond", inputPort);
    }
    if (asSimple) {
        #TODO find a way to tell it is a simple value for tuple and set 
    } else {
        #avoid MULTIPLE values
        if (is.numeric(values) && (length(values)>1)) {
            class(values) <- "VleTUPLE";
        }
    }
    rvle.setValueCondition(rvle_recursive@sim, cond="cond", port=inputPort, 
            values);
}


#'
#' Get value of input
#' 
#' @export
#' 
#' @param input, a string of the form 'cond.port'
#' @param replicate (optionnal), if TRUE the input is considered as a replicate
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
rvle.recursive.getInput = function(input=NULL, replicate=FALSE)
{
    inputPort = NULL;
    
    if (replicate) {
        inputPort = paste("replicate", sep="_", input);
    } else {
        inputPort = paste("input", sep="_", input);
    }
    
    ##try to find into rvle_recursive
    if (length(which(rvle.listConditionPorts(rvle_recursive@sim,"cond") == 
                            inputPort)) > 0) {
        return(rvle.getConditionPortValues(rvle_recursive@sim, 
                        "cond", inputPort));
    }
    ##try to find into embedded sim
    ftmp = new("Rvle", pkg=getDefault(rvle_recursive, "cond.package"), 
            file=getDefault(rvle_recursive, "cond.vpz"));
    return (getDefault(ftmp, input));
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
#' @param aggregation, amongst "mean", "quantile"
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
rvle.recursive.configOutput = function(id=NULL, path=NULL, 
        mse_observations=NULL, mse_times=NULL, integration="last", 
        aggregation="mean")
{
    if (is.null(rvle_recursive)) {
        print("[rvle.recursive] error missing initialization");
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
                aggregation=aggregation, mse_times = mse_times,
                mse_observations = mse_observations);
    } else {
        
        config = list(id=id, path=path, integration=integration, 
                aggregation=aggregation);   
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

rvle.recursive.configSimulation = function(config_parallel_type=NULL, 
        config_parallel_rm_files=NULL, config_parallel_nb_slots=NULL, 
        working_dir=NULL)
{
    if (is.null(rvle_recursive)) {
        print("[rvle.recursive] error missing initialization");
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
    rvle.setValueCondition(rvle_recursive@sim, cond="cond", 
            port="config_parallel_type", config_parallel_type);
    rvle.setValueCondition(rvle_recursive@sim, cond="cond", 
            port="config_parallel_rm_files", config_parallel_rm_files);
    rvle.setValueCondition(rvle_recursive@sim, cond="cond", 
            port="config_parallel_nb_slots", 
            as.integer(config_parallel_nb_slots));
    rvle.setValueCondition(rvle_recursive@sim, cond="cond", 
            port="working_dir", working_dir);
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

rvle.recursive.simulate = function()
{
    if (is.null(rvle_recursive)) {
        print("[rvle.recursive] error missing initialization");
        return (NULL);
    }
    res = rvle.run(rvle_recursive@sim)$view$"vle-recursive:vle_recursive.outputs"[[1]];
    header = unlist(res[1,])
    if (nrow(res)>2) {
        res = as.data.frame(res[c(2:nrow(res)),]);
    } else {
        #handle specific case where data.frame unlist results
        res = as.data.frame(rbind(NULL, res[2,]));
    }
    colnames(res) <- header;
    return (res)
}