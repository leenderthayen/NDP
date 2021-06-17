def info(object, spacing=10, collapse=1):
    """Print methods and doc strings.

    Takes module, class, list, dictionary, or string."""
    methodList = [method for method in dir(object) if callable(getattr(object, method))]
    varList = [var for var in dir(object) if not callable(getattr(object, var))]
    processFunc = collapse and (lambda s: " ".join(s.split())) or (lambda s: s)
    print("Methods")
    print("\n".join(["%s %s" %
                      (method.ljust(spacing),
                       processFunc(str(getattr(object, method).__doc__)))
                     for method in methodList]))
    print("Variables")
    print("\n".join(["%s %s" %
                      (var.ljust(spacing),
                       processFunc(str(getattr(object, var).__doc__)))
                     for var in varList]))
