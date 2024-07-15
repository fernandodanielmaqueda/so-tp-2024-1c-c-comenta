/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "module.h"

t_log *MINIMAL_LOGGER;
char *MINIMAL_LOG_PATHNAME = "minimal.log";

t_log *SOCKET_LOGGER;
char *SOCKET_LOG_PATHNAME = "socket.log";

t_log *SERIALIZE_LOGGER;
char *SERIALIZE_LOG_PATHNAME = "serialize.log";

void initialize_loggers(void) {
	MINIMAL_LOGGER = log_create(MINIMAL_LOG_PATHNAME, "Minimal", true, LOG_LEVEL_TRACE);
	MODULE_LOGGER = log_create(MODULE_LOG_PATHNAME, MODULE_NAME, true, LOG_LEVEL_TRACE);
	SOCKET_LOGGER = log_create(SOCKET_LOG_PATHNAME, "Socket", true, LOG_LEVEL_TRACE);
	SERIALIZE_LOGGER = log_create(SERIALIZE_LOG_PATHNAME, "Serialize", true, LOG_LEVEL_TRACE);
}

void finish_loggers(void) {
	log_destroy(MINIMAL_LOGGER);
	log_destroy(MODULE_LOGGER);
	log_destroy(SOCKET_LOGGER);
	log_destroy(SERIALIZE_LOGGER);
}

void initialize_configs(char *pathname) {
	MODULE_CONFIG = config_create(pathname);

	if(MODULE_CONFIG == NULL) {
		log_error(MODULE_LOGGER, "%s: No se pudo abrir el archivo de configuracion", pathname);
        exit(EXIT_FAILURE);
	}

	read_module_config(MODULE_CONFIG);
}

void finish_configs(void) {
	config_destroy(MODULE_CONFIG);
}

void *list_remove_by_condition_with_comparation(t_list *list, bool (*condition)(void *, void *), void *comparation) {
	t_link_element **indirect = &(list->head);
	while (*indirect != NULL) {
		if(condition(((*indirect)->data), comparation)) {
			t_link_element *removed_element = *indirect;

			*indirect = (*indirect)->next;
			list->elements_count--;

			void *data = removed_element->data;
			free(removed_element);
			return data;
		}
		indirect = &((*indirect)->next);
	}
	return NULL;
}

int list_add_unless_matches_with_any(t_list *list, void *data, bool (*condition)(void *, void*)) {
    t_link_element **indirect = &(list->head);
    while (*indirect != NULL) {
        if(condition((*indirect)->data, data))
            return 1;
        indirect = &((*indirect)->next);
    }

    t_link_element *new_element = (t_link_element *) malloc(sizeof(t_link_element));
    if(new_element == NULL) {
        log_error(MODULE_LOGGER, "malloc: No se pudo reservar memoria para [Elemento] de [Interfaz] Entrada/Salida");
        return 1;
    }

    new_element->data = data;
    new_element->next = NULL;

    *indirect = new_element;
    list->elements_count++;

    return 0;
}