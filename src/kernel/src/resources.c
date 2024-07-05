/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "resources.h"

t_Resource *RESOURCES;

t_list *ASSIGNED_RESOURCES;

int RESOURCE_QUANTITY;

void resources_read_module_config(t_config *module_config) {
	char **resource_names = config_get_array_value(module_config, "RECURSOS");
	char **resource_instances = config_get_array_value(module_config, "INSTANCIAS_RECURSOS");

	for(RESOURCE_QUANTITY = 0; (resource_names[RESOURCE_QUANTITY] != NULL) && (resource_instances[RESOURCE_QUANTITY] != NULL); RESOURCE_QUANTITY++);
	
	if((resource_names[RESOURCE_QUANTITY] != NULL) || (resource_instances[RESOURCE_QUANTITY] != NULL)) {
		log_error(MODULE_LOGGER, "La cantidad de recursos y de instancias de recursos no coinciden");
		exit(EXIT_FAILURE);
	}

	RESOURCES = malloc(sizeof(t_Resource) * (RESOURCE_QUANTITY + 1));
	if(RESOURCES == NULL) {
		log_error(MODULE_LOGGER, "No se pudo reservar memoria para los recursos");
		exit(EXIT_FAILURE);
	}

	char *end;
	long instances;

	for(int i = 0; i < RESOURCE_QUANTITY; i++) {
		instances = strtol(resource_instances[i], &end, 10);
		if(!*(resource_instances[i]) || *end) {
			log_error(MODULE_LOGGER, "La cantidad de instancias del recurso %s no es un número válido: %s", resource_names[i], resource_instances[i]);
			exit(EXIT_FAILURE);
		}

		RESOURCES[i].name = resource_names[i];
		RESOURCES[i].available = instances;
		RESOURCES[i].total = instances;
		RESOURCES[i].list_blocked = list_create();
		pthread_mutex_init(&(RESOURCES[i].mutex_list_blocked), NULL);
	}

	RESOURCES[RESOURCE_QUANTITY].name = NULL;
}

void resources_free(void) {
	free(RESOURCES);
}

void resource_log(t_Resource *resource) {
	log_trace(MODULE_LOGGER, "Recurso: %s - Instancias disponibles %ld/%ld", resource->name, resource->available, resource->total);
}

t_Resource *resource_find(char *name) {
    for(register int i = 0; RESOURCES[i].name != NULL; i++)
        if(!strcmp(RESOURCES[i].name, name))
            return (&RESOURCES[i]);

    return NULL;
}