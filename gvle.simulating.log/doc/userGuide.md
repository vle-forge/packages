The package **gvle.simulating.log** provide a simulation plugin that does enable to log
inside the gui outputs.

### Usage

The simulation plugin expect to find a storage view in order to displays the result.
The plugin proposes on the right side of the gui an area to choose the variables the user might want to display.

In order to use the plugin, the user has to define a storage view, and an observable that defines all the observable ports that can be observed.
Each port has to be linked to the storage view.

### Current limitation

When configuring the storage view, either static or dynamic dimension parameters have to be disable(set to 0).