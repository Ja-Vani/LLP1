//
// Created by Ja_Vani on 12.03.2023.
//

#include <string.h>
#include <time.h>
#include <unistd.h>
#include "test_database.h"

#define INTERACTION 100

void init_selectors_test(FILE *f) {
    char person[8] = "person0\0";
    for (int i = 0; i < 10; i++) {
        node *n = create_node("person",
                              create_attribute("name", string, person, NULL),
                              create_edge("know", (i + 1) % 10,
                                          create_edge("write", 10 + (i + 1) % 10, NULL)));
        add_node(f, n);
        free_node(n);
        person[6]++;
    }
    char text[6] = "text0\0";
    for (int i = 0; i < 10; i++) {
        node *n = create_node("text",
                              create_attribute("name", string, text, NULL),
                              create_edge("writer", (i + 1) % 10, NULL));
        add_node(f, n);
        free_node(n);
        text[4]++;
    }
}

void filters_test(FILE *f) {
    attribute *att = create_attribute("name", string, "person3", NULL);
    struct filter *filter = create_filter(equals, att, NULL);
    graph *g = find_filter(f, filter);
    if (g->n == 1 && !strcmp(g->nodes->name, "person")) {
        fprintf(stderr, "filters test passed\n");
    } else {
        fprintf(stderr, "filters test not passed\n");
        fprintf(stderr, "find %d elements\n", g->n);
    }
    free_graph(g);
    free_attribute(att);
    free_filter(filter);
}

void add_test(FILE *f) {
    node *n = create_node("text",
                          create_attribute("name", string, "book", NULL),
                          create_edge("writer", 0, NULL));
    add_node(f, n);
    free_node(n);
    int a = 132;
    attribute *att = create_attribute("size", integer, &a, NULL);
    add_attribute(f, att, 20);
    edge *edge = create_edge("writer", 1, NULL);
    add_edge(f, edge, 20);
    n = get_node(f, 20);
    if (!strcmp(n->att->sfield, "book") && n->att->next->ifield == a && n->edges->id == 1 && n->edges->next->id == 0) {
        fprintf(stderr, "add test passed\n");
    } else {
        fprintf(stderr, "add test not passed\n");
    }
    free_node(n);
}

void update_test(FILE *f) {
    node *n = get_node(f, 20);
    if (n->att->next->type == integer)n->att->next->ifield = 10;
    update_node(f, n);
    free_node(n);
    n = get_node(f, 20);
    if (n->att->ifield == 10) {
        fprintf(stderr, "update test passed\n");
    } else {
        fprintf(stderr, "update test not passed\n");
    }
}

void delete_test(FILE *f) {
    delete_node(f, 20);
    graph *g = find_by_name(f, "text");
    if (g->n == 10) {
        fprintf(stderr, "delete test passed\n");
    } else {
        fprintf(stderr, "delete test not passed\n");
    }
    free_graph(g);
}

void selectors_test() {
    FILE *f = fopen("selectors_test", "w");
    fclose(f);
    f = open_database("selectors_test");
    init_selectors_test(f);
    filters_test(f);
    add_test(f);
    update_test(f);
    delete_test(f);
    fclose(f);
}

void update_performance_test(FILE *f) {
    node *n = get_node(f, 0);
    n->att->name = "update";
    update_node(f, n);
    free_node(n);
}

void delete_performance_test(FILE *f) {
    for (int i = 0; i < 20; i++) {
        delete_node(f, i);
    }
}

void find_performance_test(FILE *f){
    attribute *att = create_attribute("name", string, "person3", NULL);
    struct filter *filter = create_filter(equals, att, NULL);
    graph *g = find_filter(f, filter);
    free_graph(g);
    free_attribute(att);
    free_filter(filter);
}

void edge_test(FILE *f){
    graph *g = find_edge(f, "know", 0);
    free_graph(g);
}

void ref_test(FILE *f){
    struct reference ref;
    ref.ref_id = 0;
    ref.name = "name";
    attribute *att = create_attribute("name", reference, &ref, NULL);
    struct filter *filter = create_filter(equals, att, NULL);
    graph *g = find_filter(f, filter);
    free_graph(g);
    free_attribute(att);
    free_filter(filter);
}

void performance_test() {
    FILE *f = fopen("performance_test", "w");
    fclose(f);
    f = open_database("performance_test");
    FILE *insert = fopen("insert_stat", "w");
    FILE *update = fopen("update_stat", "w");
    FILE *delete = fopen("delete_stat", "w");
    FILE *find = fopen("find_stat", "w");
    FILE *edge = fopen("edge_stat", "w");
    FILE *ref = fopen("ref_stat", "w");
    for (int i = 0; i < INTERACTION; i++) {
        clock_t begin = clock();
        init_selectors_test(f);
        clock_t end = clock();
        double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
        fprintf(insert, "%lf\n", time_spent);

        begin = clock();
        update_performance_test(f);
        end = clock();
        time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
        fprintf(update, "%lf\n", time_spent);

        begin = clock();
        find_performance_test(f);
        end = clock();
        time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
        fprintf(find, "%lf\n", time_spent);

        begin = clock();
        edge_test(f);
        end = clock();
        time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
        fprintf(edge, "%lf\n", time_spent);

        begin = clock();
        ref_test(f);
        end = clock();
        time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
        fprintf(ref, "%lf\n", time_spent);

        init_selectors_test(f);
        begin = clock();
        delete_performance_test(f);
        end = clock();
        time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
        fprintf(delete, "%lf\n", time_spent);
        fprintf(stderr, "iteration: %d\n", i);
    }
    fclose(insert);
    fclose(update);
    fclose(delete);
    fclose(edge);
    fclose(find);
    fclose(f);
}