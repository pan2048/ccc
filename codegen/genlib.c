// Generate and return a new label number
static int labelid = 1;
int getLabel() {
  return (labelid++);
}