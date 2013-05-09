=head1 NAME

Bi::Gen::Bi - generator for Bi model specifications.

=head1 SYNOPSIS

    use Bi::Gen::Bi;
    $gen = new Bi::Gen::Bi($outdir);
    $gen->gen($model);

=head1 INHERITS

L<Bi::Gen>

=head1 METHODS

=over 4

=cut

package Bi::Gen::Bi;

use parent 'Bi::Gen';
use warnings;
use strict;

use Carp::Assert;
use File::Spec;

use Bi qw(share_file share_dir);
use Bi::Visitor::ToAscii;

=item B<new>

Constructor.

=cut
sub new {
    my $class = shift;

    my $ttdir = share_dir(File::Spec->catdir('tt', 'bi'));
    my $self = Bi::Gen->new($ttdir);

    $self->get_tt->context->define_vmethod( 'hash', 'to_bi', \&to_bi);

    bless $self, $class;    
    return $self;
}

=item B<gen>(I<model>)

Generate code for model.

=cut

sub gen {
    my $self = shift;
    my $model = shift;

    # pre-condition
    assert(defined($model) && $model->isa('Bi::Model')) if DEBUG;

    # model
    $self->process_templates('model', { 'model' => $model });
}

=item B<process_templates>(I<template_name>, I<vars>, I<output_name>)

Process all template files which have the given name, plus a file extension
of C<.bi.tt>.

=cut
sub process_templates {
    my $self = shift;
    my $template_name = shift;
    my $vars = shift;
    
    my @ext = ('bi');
    my $ext;
    
    foreach $ext (@ext) {
        if ($self->is_template("$template_name.$ext.tt")) {
            $self->process_template("$template_name.$ext.tt", $vars, \*STDOUT);
        }   
    }
}

=back

=head1 CLASS METHODS

=over 4

=item B<to_bi>(I<expr>)

Bi expression filter.

=cut
sub to_bi {
    my $expr = shift;
    return Bi::Visitor::ToAscii->evaluate($expr);
}

1;

=back

=head1 AUTHOR

Lawrence Murray <lawrence.murray@csiro.au>

=head1 VERSION

$Rev$ $Date$
