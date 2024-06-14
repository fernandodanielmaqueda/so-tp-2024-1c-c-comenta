/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "resources.h"

t_Resource *RESOURCES;

void resources_read_module_config(t_config *module_config) {
	char **resource_names = config_get_array_value(module_config, "RECURSOS");
	char **resource_instances = config_get_array_value(module_config, "INSTANCIAS_RECURSOS");

	register int i;
	for(i = 0; (resource_names[i] != NULL) && (resource_instances[i] != NULL); i++);
	
	if((resource_names[i] != NULL) || (resource_instances[i] != NULL)) {
		log_error(MODULE_LOGGER, "La cantidad de recursos y de instancias de recursos no coinciden");
		exit(EXIT_FAILURE);
	}

	RESOURCES = malloc(sizeof(t_Resource) * (i + 1));
	if(RESOURCES == NULL) {
		log_error(MODULE_LOGGER, "No se pudo reservar memoria para los recursos");
		exit(EXIT_FAILURE);
	}

	char *end;

	for(int j = 0; j < i; j++) {
		RESOURCES[j].name = resource_names[j];
		RESOURCES[j].instances = strtol(resource_instances[j], &end, 10);
		if(!*(resource_instances[j]) || *end) {
			log_error(MODULE_LOGGER, "La cantidad de instancias del recurso %s no es un número válido: %s", resource_names[j], resource_instances[j]);
			exit(EXIT_FAILURE);
		}
	}

	RESOURCES[i].name = NULL;
}

void resources_free(t_Resource *resources) {
	free(resources);
}

t_Resource *find_resource (char *name) {
    for(register int i = 0; RESOURCES[i].name != NULL; i++)
        if(!strcmp(RESOURCES[i].name, name))
            return (&RESOURCES[i]);

    return NULL;
}