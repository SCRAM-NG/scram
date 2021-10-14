.. _probability_analysis:

####################
Probability Analysis
####################

*****************
Probability Types
*****************

Various probability types and distributions are accepted
as described in the Open-PSA Model Exchange Format `[MEF] <references.rst#standards-guides-manuals-handbooks>`__,
for example, constant values, exponential with two or four parameters,
and uniform, normal, log-normal distributions.

Bellow is a brief description.
For more information, please take a look at the `[MEF] <references.rst#standards-guides-manuals-handbooks>`__ format documentation.


************************
Probability Calculations
************************

Probability calculation algorithms assume
independence of basic events in the fault tree.
The dependence can be communicated with common cause groups.


The Exact Probability Calculation
=================================

Since the resultant sets may neither be mutually exclusive nor independent,
direct use of the sets' total probabilities may be inaccurate `[WakXX] <references.rst#papers>`__.
The exact probability calculation is achieved
with Binary Decision Diagram (`[BDD] <references.rst#web-resources>`__) based algorithms `[DR01] <references.rst#papers>`__.
This approach does not require calculation of products.
As long as a fault tree (`[PDAG] <references.rst#web-resources>`__) can be converted into BDD,
the calculation of its probability is linear in the size of BDD.


The Approximate Probability Calculation
=======================================

Approximate calculations are implemented to reduce the calculation time.
However, the users must be aware of the limitations and inaccuracies of approximations `[WakXX] <references.rst#papers>`__.
If approximate calculations yield probability values above one,
the result is adjusted to one.
In this special case,
appropriate warnings are given in the final report.


The Rare-Event Approximation
----------------------------

Given that the probabilities of events are very small value (less than 0.1),
only the first series in the Sylvester-Poincaré formula may be used
as a conservative (upper-bound) approximation;
that is, the total probability is the sum of all probabilities of minimal cut sets.
Ideally, this approximation gives good results
for independent minimal cut sets with very low probabilities.
However, if the cut set probabilities are high,
the total probability may exceed 1.

This is the default quantitative approximation
for analysis algorithms that inherently run with approximations (MOCUS and ZBDD).


The Min-Cut-Upper-Bound (MCUB) Approximation
--------------------------------------------

This method calculates the total probability
by subtracting the probability of all minimal cut sets' being successful from 1;
thus, the total probability never exceeds 1.
Non-independence of the minimal cut sets introduce the major discrepancy for this technique.
Moreover, the MCUB approximation provides non-conservative estimation
for non-coherent trees containing NOT logic `[WakXX] <references.rst#papers>`__.


*******************
Importance Analysis
*******************

Importance analysis is performed for basic events in a fault tree `[DR01] <references.rst#papers>`__.
The same configurations are used as for probability analysis.
The analysis is performed by request with probability data.
The following importance factors are calculated:

- Fussel-Vesely Diagnosis Importance Factor (DIF)
- Birnbaum Marginal Importance Factor (MIF)
- Critical Importance Factor (CIF)
- Risk Reduction Worth (RRW)
- Risk Achievement Worth (RAW)

The short description and interpretation of the factors
can be found in `theory <theory.rst>`__.

Alongside the importance factors,
the analysis provides the probabilities of events and their number of occurrences in products.


***********************
Safety Integrity Levels
***********************

`[IEC_61508] <references.rst#web-resources>`__ standard metrics and Safety Integrity Levels `[SIL] <references.rst#web-resources>`__
are approximated with quantitative analysis on fault trees `[DR05] <references.rst#papers>`__.
Time fractions spent in every SIL bucket for PFD and PFH
are reported with a histogram,
as suggested by `[DRS08] <references.rst#papers>`__.
Note that these computations require probability analysis over a period of time.

.. warning::
    The current implementation for the PFH calculation is simplistic,
    resulting in potentially less accurate values
    than the more rigorous approaches suggested in the above papers.
    The PFH results may become more inaccurate
    with testable, repairable, and/or non-continuously-operated components.
    At best, the approximate value is expected to be of the same magnitude as the real value,
    which puts the approximation into the same Safety Integrity Level.
